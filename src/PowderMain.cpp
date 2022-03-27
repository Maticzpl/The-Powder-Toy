#include "gui/SDLWindow.h"
#include "activities/game/Game.h"
#include "graphics/ThumbnailRenderer.h"
#include "network/RequestManager.h"
#include "backend/Backend.h"
#include "backend/Startup.h"
#include "prefs/GlobalPrefs.h"
#include "prefs/StampPrefs.h"
#include "language/Language.h"
#include "common/Worker.h"

#include <stdexcept>
#include <iostream>
#include <map>
#include <csignal>
#include <cstdlib>
#ifdef X86_SSE
# include <xmmintrin.h>
#endif
#ifdef X86_SSE3
# include <pmmintrin.h>
#endif
#ifdef WIN
# include <direct.h>
#else
# include <unistd.h>
#endif
#ifdef MACOSX
# include <CoreServices/CoreServices.h>
#endif
#include <sys/stat.h>

#if !defined(DEBUG) && !defined(_DEBUG)
# define ENABLE_BLUESCREEN
#endif

using ArgumentMap = std::map<ByteString, ByteString>;

static ArgumentMap readArguments(int argc, char *argv[])
{
	std::map<ByteString, ByteString> arguments;
	arguments["scale"] = "";
	arguments["proxy"] = "";
	arguments["kiosk"] = "";
	arguments["redirect"] = "";
	arguments["open"] = "";
	arguments["ddir"] = "";
	arguments["ptsave"] = "";
	arguments["disable-network"] = "";
	arguments["renderer-type"] = "";
	for (int i = 1; i < argc; ++i)
	{
		ByteString arg(argv[i]);
		auto split = arg.SplitBy(':');
		auto key = split ? split.Before() : arg;
		auto value = split ? split.After() : ByteString("true");
		auto it = arguments.find(key);
		if (it != arguments.end())
		{
			if (key == "open" || key == "ddir" || key == "ptsave")
			{
				if (i + 1 < argc)
				{
					i += 1;
					it->second = argv[i];
				}
				else
				{
					std::cerr << "argument #" << i << " ignored: no value provided" << std::endl;
				}
			}
			else
			{
				it->second = value;
			}
		}
		else
		{
			std::cerr << "argument #" << i << " ignored: unrecognized option" << std::endl;
		}
	}
	return arguments;
}

static void doDdir(const ArgumentMap &arguments)
{
	if (arguments.at("ddir").length())
	{
#ifdef WIN
		int failure = _chdir(arguments.at("ddir").c_str());
#else
		int failure = chdir(arguments.at("ddir").c_str());
#endif
		if (failure)
		{
			perror("failed to chdir to requested ddir");
		}
	}
	else
	{
#ifdef WIN
		struct _stat s;
		if (_stat("powder.pref", &s))
#else
		struct stat s;
		if (stat("powder.pref", &s))
#endif
		{
			auto ddir = gui::SDLWindow::GetPrefPath();
			if (ddir.size())
			{
#ifdef WIN
				int failure = _chdir(ddir.c_str());
#else
				int failure = chdir(ddir.c_str());
#endif
				if (failure)
				{
					perror("failed to chdir to default ddir");
				}
			}
		}
	}
}

static void doRedirect(const ArgumentMap &arguments)
{
	if (arguments.at("redirect") == "true")
	{
		FILE *newStdout = freopen("stdout.log", "w", stdout);
		FILE *newStderr = freopen("stderr.log", "w", stderr);
		if (!newStdout || !newStderr)
		{
			// * No point in printing an error to stdout/stderr since the user
			//   probably requests those streams be redirected because they
			//   can't see them otherwise. So just throw an exception instead
			//   and hope that the OS and the standard library is smart enough
			//   to display the error message in some useful manner.
			throw std::runtime_error("cannot honour request to redirect standard streams to log files");
		}
	}
}

static void doScale(const ArgumentMap &arguments, int &scale)
{
	if (arguments.at("scale").length())
	{
		try
		{
			scale = arguments.at("scale").ToNumber<int>();
		}
		catch (const std::runtime_error &)
		{
		}
		prefs::GlobalPrefs::Ref().Set("Scale", scale);
	}
}

static void doKiosk(const ArgumentMap &arguments, bool &fullscreen)
{
	if (arguments.at("kiosk").length())
	{
		fullscreen = arguments.at("kiosk") == "true";
		prefs::GlobalPrefs::Ref().Set("Fullscreen", fullscreen);
	}
}

static void doProxy(const ArgumentMap &arguments, ByteString &proxy)
{
	if (arguments.at("proxy").length())
	{
		proxy = arguments.at("proxy");
		if (proxy == "false")
		{
			proxy = "";
		}
		prefs::GlobalPrefs::Ref().Set("Proxy", proxy);
	}
}

static void doDisableNetwork(const ArgumentMap &arguments, bool &disableNetwork)
{
	if (arguments.at("disable-network").length())
	{
		disableNetwork = arguments.at("disable-network") == "true";
	}
}

static void doRendererType(const ArgumentMap &arguments, gui::SDLWindow::Configuration::RendererType &rendererType)
{
	if (arguments.at("renderer-type").length())
	{
		if (arguments.at("renderer-type") == "software")
		{
			rendererType = gui::SDLWindow::Configuration::rendererSoftware;
		}
		else if (arguments.at("renderer-type") == "accelerated")
		{
			rendererType = gui::SDLWindow::Configuration::rendererAccelerated;
		}
		else
		{
			rendererType = gui::SDLWindow::Configuration::rendererDefault;
		}
		prefs::GlobalPrefs::Ref().Set("RendererType", int(rendererType));
	}
}

#ifdef ENABLE_BLUESCREEN
static void sigHandler(int sig)
{
	switch (sig)
	{
	case SIGSEGV:
		gui::SDLWindow::Ref().BlueScreen("Memory read/write error");
		break;

	case SIGFPE:
		gui::SDLWindow::Ref().BlueScreen("Floating point exception");
		break;

	case SIGILL:
		gui::SDLWindow::Ref().BlueScreen("Invalid instruction");
		break;

	case SIGABRT:
		gui::SDLWindow::Ref().BlueScreen("Unexpected program abort");
		break;
	}
}
#endif

static void frame()
{
	auto &sdlw = gui::SDLWindow::Ref();
	sdlw.FrameBegin();
	sdlw.FramePoll();
	if (!sdlw.Running())
	{
		return;
	}
#ifndef NOHTTP
	network::RequestManager::Ref().Tick();
#endif
	backend::Backend::Ref().Tick();
	backend::Startup::Ref().Tick();
	sdlw.FrameTick();
	sdlw.FrameDraw();
	sdlw.FrameEnd();
	sdlw.FrameDelay();
	sdlw.FrameTime();
}

#ifdef main
# undef main
#endif

int main(int argc, char *argv[])
{
#if defined(_DEBUG) && defined(_MSC_VER)
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif

#ifdef ENABLE_BLUESCREEN
	// * Get ready to catch any dodgy errors.
	signal(SIGSEGV, sigHandler);
	signal(SIGFPE, sigHandler);
	signal(SIGILL, sigHandler);
	signal(SIGABRT, sigHandler);
#endif

#ifdef X86_SSE
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif
#ifdef X86_SSE3
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif

	ArgumentMap arguments = readArguments(argc, argv);
	doRedirect(arguments);
	doDdir(arguments);

	prefs::GlobalPrefs gpref; // * Needs to happen after doDdir.
	prefs::StampPrefs spref; // * Needs to happen after doDdir.
	language::Language lang(gpref.Get("Language", String("en_US")));
	network::RequestManager rm;
	backend::Backend be;
	backend::Startup su;

	gui::SDLWindow::Configuration sdlwConf;
	sdlwConf.size = { WINDOWW, WINDOWH };
	sdlwConf.scale = gpref.Get("Scale", 1);
	sdlwConf.fullscreen   = gpref.Get("Fullscreen",          false);
	sdlwConf.borderless   = gpref.Get("AltFullscreen",       false);
	sdlwConf.resizable    = gpref.Get("Resizable",           false);
	sdlwConf.blurry       = gpref.Get("Blurry",              false);
	sdlwConf.integer      = gpref.Get("ForceIntegerScaling",  true);
	sdlwConf.rendererType = gpref.Get("RendererType", gui::SDLWindow::Configuration::rendererMax, gui::SDLWindow::Configuration::rendererDefault);
	doScale(arguments, sdlwConf.scale);
	doKiosk(arguments, sdlwConf.fullscreen);
	doRendererType(arguments, sdlwConf.rendererType);

	// * TODO-REDO_UI: Replicate guess best scale behaviour.
	// * TODO-REDO_UI: Replicate open behaviour.
	// * TODO-REDO_UI: Replicate ptsave behaviour.
	// * TODO-REDO_UI: Replicate window position behaviour.
	// * TODO-REDO_UI: Replicate draw limit behaviour.

	ByteString proxy = gpref.Get("Proxy", ByteString(""));
	bool disableNetwork = false;
	doProxy(arguments, proxy);
	doDisableNetwork(arguments, disableNetwork);

	// cli.Initialise(proxy, disableNetwork); // * TODO-REDO_UI
	
	{
		gui::SDLWindow sdlw(sdlwConf);
		graphics::ThumbnailRenderer tr; // Uses gui::SDLWindow, so gui::SDLWindow has to outlive it.
		common::Worker worker; // May use any other ExplicitSingleton, should be constructed last.
		sdlw.MomentumScroll(gpref.Get("MomentumScroll", true));
		sdlw.FastQuit(gpref.Get("FastQuit", true));
		sdlw.TimeFormat(gpref.Get("TimeFormat", String("%c")));

#ifdef ENABLE_BLUESCREEN
		try
#endif
		{
			sdlw.EmplaceBack<activities::game::Game>();
			while (sdlw.Running())
			{
				frame();
			}
		}
#ifdef ENABLE_BLUESCREEN
		catch (const std::exception &ex)
		{
			gui::SDLWindow::Ref().BlueScreen(ByteString(ex.what()).FromUtf8());
		}
#endif
	}

	return 0;
}
