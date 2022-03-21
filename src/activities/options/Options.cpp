#include "Options.h"

#include "common/Platform.h"
#include "gui/SDLWindow.h"
#include "gui/Appearance.h"
#include "gui/Button.h"
#include "gui/CheckBox.h"
#include "gui/DropDown.h"
#include "gui/TextBox.h"
#include "gui/ScrollPanel.h"
#include "gui/Separator.h"
#include "gui/Static.h"
#include "graphics/Pix.h"
#include "simulation/ElementDefs.h"
#include "graphics/SimulationRenderer.h"
#include "prefs/GlobalPrefs.h"
#include "activities/game/Game.h"
#include "language/Language.h"

float ParseFloatProperty(String value)
{
	if (value.EndsWith("C"))
	{
		float v = value.SubstrFromEnd(1).ToNumber<float>();
		return v + 273.15;
	}
	if (value.EndsWith("F"))
	{
		float v = value.SubstrFromEnd(1).ToNumber<float>();
		return (v - 32.0f) * 1.8f + 273.15f;
	}
	return value.ToNumber<float>();
}

namespace activities::options
{
	constexpr auto parentSize = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto windowSize = gui::Point{ 320, 341 };
	constexpr auto simDropWidth = 80;

	Options::Options()
	{
		// * TODO-REDO_UI: Figure out where and when to save preferences.
		// * TODO-REDO_UI: data direction migration

		auto &gm = game::Game::Ref();
		Size(windowSize);
		Position((parentSize - windowSize) / 2);

		{
			auto title = EmplaceChild<gui::Static>();
			title->Position({ 6, 5 });
			title->Size({ windowSize.x - 20, 14 });
			title->Text("DEFAULT_LS_OPTIONS_TITLE"_Ls());
			title->TextColor(gui::Appearance::informationTitle);

			auto sep = EmplaceChild<gui::Separator>();
			sep->Position({ 1, 22 });
			sep->Size({ windowSize.x - 2, 1 });

			auto ok = EmplaceChild<gui::Button>();
			ok->Position({ 0, windowSize.y - 17 });
			ok->Size({ windowSize.x, 17 });
			ok->Text(String::Build(gui::CommonColorString(gui::commonYellow), "DEFAULT_LS_OPTIONS_OK"_Ls()));
			ok->Click([this]() {
				Quit();
			});
			Cancel([ok]() {
				ok->TriggerClick();
			});
			Okay([ok]() {
				ok->TriggerClick();
			});
		}

		auto sp = EmplaceChild<gui::ScrollPanel>();
		sp->Position({ 1, 23 });
		sp->Size(windowSize - sp->Position() - gui::Point{ 1, 17 });

		auto *spi = sp->Interior(); // * It's bigger on the inside.
		auto interorHeight = 8;

		auto simCheck = [&interorHeight, spi](String name, String introduced, String description, bool value, gui::CheckBox::ChangeCallback cb) {
			auto checkbox = spi->EmplaceChild<gui::CheckBox>();
			checkbox->Position({ 10, interorHeight });
			checkbox->Size({ windowSize.x - 20, 14 });
			checkbox->Text(name + " \bg" + introduced);
			checkbox->Change(cb);
			checkbox->Value(value);
			auto label = spi->EmplaceChild<gui::Static>();
			label->Position({ 26, interorHeight + 14 });
			label->Size({ windowSize.x - 36, 14 });
			label->Text("\bg" + description);
			interorHeight += 30;
		};

		auto simDrop = [&interorHeight, spi](String name, std::vector<String> options, int value, gui::DropDown::ChangeCallback cb) {
			auto label = spi->EmplaceChild<gui::Static>();
			label->Position({ 10, interorHeight });
			label->Size({ windowSize.x - 29 - simDropWidth, 14 });
			label->Text(name);
			auto dropdown = spi->EmplaceChild<gui::DropDown>();
			dropdown->Position({ windowSize.x - simDropWidth - 15, interorHeight - 1 });
			dropdown->Size({ simDropWidth, 16 });
			dropdown->Options(options);
			dropdown->Change(cb);
			dropdown->Value(value);
			interorHeight += 20;
		};

		auto separator = [&interorHeight, spi]() {
			auto sep = spi->EmplaceChild<gui::Separator>();
			sep->Position({ 0, interorHeight - 4 });
			sep->Size({ windowSize.x - 2, 1 });
			interorHeight += 8;
		};

		auto otherCheck = [&interorHeight, spi](int indent, String name, String description, bool value, gui::CheckBox::ChangeCallback cb) {
			indent *= 15;
			auto checkbox = spi->EmplaceChild<gui::CheckBox>();
			checkbox->Position({ 10 + indent, interorHeight });
			checkbox->Size({ windowSize.x - 20, 14 });
			checkbox->Text(name + " \bg- " + description);
			checkbox->Change(cb);
			checkbox->Value(value);
			interorHeight += 20;
		};

		auto otherDrop = [&interorHeight, spi](int indent, int width, String description, std::vector<String> options, int value, gui::DropDown::ChangeCallback cb) {
			auto label = spi->EmplaceChild<gui::Static>();
			label->Position({ 15 + width, interorHeight });
			label->Size({ windowSize.x - 30 - width, 14 });
			label->Text("\bg- " + description);
			auto dropdown = spi->EmplaceChild<gui::DropDown>();
			dropdown->Position({ 10, interorHeight - 1 });
			dropdown->Size({ width, 16 });
			dropdown->Options(options);
			dropdown->Change(cb);
			dropdown->Value(value);
			interorHeight += 20;
		};

		auto otherButton = [&interorHeight, spi](int indent, int width, String title, String description, gui::Button::ClickCallback cb) {
			auto label = spi->EmplaceChild<gui::Static>();
			label->Position({ 15 + width, interorHeight });
			label->Size({ windowSize.x - 30 - width, 14 });
			label->Text("\bg- " + description);
			auto button = spi->EmplaceChild<gui::Button>();
			button->Position({ 10, interorHeight - 1 });
			button->Size({ width, 16 });
			button->Text(title);
			button->Click(cb);
			interorHeight += 20;
		};

		simCheck(
			"DEFAULT_LS_OPTIONS_HEAT"_Ls(),
			"DEFAULT_LS_OPTIONS_HEAT_INTRO"_Ls(),
			"DEFAULT_LS_OPTIONS_HEAT_DESC"_Ls(),
			gm.LegacyHeat(), [](bool value) {
				game::Game::Ref().LegacyHeat(value);
			}
		);
		simCheck(
			"DEFAULT_LS_OPTIONS_AMBIENTHEAT"_Ls(),
			"DEFAULT_LS_OPTIONS_AMBIENTHEAT_INTRO"_Ls(),
			"DEFAULT_LS_OPTIONS_AMBIENTHEAT_DESC"_Ls(),
			gm.AmbientHeat(), [](bool value) {
				game::Game::Ref().AmbientHeat(value);
			}
		);
		simCheck(
			"DEFAULT_LS_OPTIONS_NEWTONIAN"_Ls(),
			"DEFAULT_LS_OPTIONS_NEWTONIAN_INTRO"_Ls(),
			"DEFAULT_LS_OPTIONS_NEWTONIAN_DESC"_Ls(),
			gm.NewtonianGravity(), [](bool value) {
				game::Game::Ref().NewtonianGravity(value);
			}
		);
		simCheck(
			"DEFAULT_LS_OPTIONS_WATEREQ"_Ls(),
			"DEFAULT_LS_OPTIONS_WATEREQ_INTRO"_Ls(),
			"DEFAULT_LS_OPTIONS_WATEREQ_DESC"_Ls(),
			gm.WaterEqualisation(), [](bool value) {
				game::Game::Ref().WaterEqualisation(value);
			}
		);
		interorHeight += 8;
		simDrop("DEFAULT_LS_OPTIONS_AIR"_Ls(), {
			"DEFAULT_LS_OPTIONS_AIR_ON"_Ls(),
			"DEFAULT_LS_OPTIONS_AIR_PRESSUREOFF"_Ls(),
			"DEFAULT_LS_OPTIONS_AIR_VELOCITYOFF"_Ls(),
			"DEFAULT_LS_OPTIONS_AIR_OFF"_Ls(),
			"DEFAULT_LS_OPTIONS_AIR_NOUPDATE"_Ls(),
		}, int(gm.AirMode()), [](int value) {
			game::Game::Ref().AirMode(game::SimAirMode(value));
		});
		{
			auto label = spi->EmplaceChild<gui::Static>();
			label->Position({ 10, interorHeight });
			label->Size({ windowSize.x - 29 - simDropWidth, 14 });
			label->Text("DEFAULT_LS_OPTIONS_AMBIENTAIRTEMP"_Ls());
			auto *tb = spi->EmplaceChild<gui::TextBox>().get();
			tb->Position({ windowSize.x - simDropWidth - 15, interorHeight - 1 });
			tb->Size({ simDropWidth - 20, 16 });
			tb->Align(gui::Alignment::horizCenter);
			auto *btn = spi->EmplaceChild<gui::Button>().get();
			btn->Position({ windowSize.x - 31, interorHeight - 1 });
			btn->Size({ 16, 16 });
			tb->Change([tb, btn]() {
				float value;
				bool valid = false;
				try
				{
					value = ParseFloatProperty(tb->Text());
					valid = true;
				}
				catch (const std::exception &ex)
				{
				}
				if (!(MIN_TEMP <= value && MAX_TEMP >= value))
				{
					valid = false;
				}
				if (valid)
				{
					game::Game::Ref().AmbientAirTemp(value);
					uint32_t c = game::Game::Ref().GetRenderer()->HeatToColour(value);
					btn->BodyColor({ PixR(c), PixG(c), PixB(c), 255 });
					btn->Text("");
				}
				else
				{
					btn->BodyColor({ 0, 0, 0, 255 });
					btn->Text("?");
				}
			});
			auto reset = [tb]() {
				StringBuilder sb;
				sb << Format::Precision(2) << game::Game::Ref().AmbientAirTemp();
				tb->Text(sb.Build());
				tb->TriggerChange();
			};
			tb->Validate(reset);
			reset();
			interorHeight += 20;
		}
		simDrop("DEFAULT_LS_OPTIONS_GRAVITY"_Ls(), {
			"DEFAULT_LS_OPTIONS_GRAVITY_VERTICAL"_Ls(),
			"DEFAULT_LS_OPTIONS_GRAVITY_RADIAL"_Ls(),
			"DEFAULT_LS_OPTIONS_GRAVITY_OFF"_Ls(),
		}, int(gm.GravityMode()), [](int value) {
			game::Game::Ref().GravityMode(game::SimGravityMode(value));
		});
		simDrop("DEFAULT_LS_OPTIONS_EDGE"_Ls(), {
			"DEFAULT_LS_OPTIONS_EDGE_VOID"_Ls(),
			"DEFAULT_LS_OPTIONS_EDGE_SOLID"_Ls(),
			"DEFAULT_LS_OPTIONS_EDGE_LOOP"_Ls(),
		}, int(gm.EdgeMode()), [](int value) {
			game::Game::Ref().EdgeMode(game::SimEdgeMode(value));
		});
		interorHeight += 8;

		separator();

		{
			std::vector<String> options;
			auto max = gui::SDLWindow::Ref().MaxScale();
			auto current = gui::SDLWindow::Ref().Config().scale;
			for (auto i = 1; i <= max; ++i)
			{
				options.push_back(String::Build(i));
			}
			bool currentValid = current <= max;
			if (!currentValid)
			{
				options.push_back("current");
			}
			auto count = int(options.size());
			otherDrop(0, 40, "DEFAULT_LS_OPTIONS_WINDOWSIZE_DESC"_Ls(), options, currentValid ? (current - 1) : (count - 1), [currentValid, current, count](int value) {
				auto conf = gui::SDLWindow::Ref().Config();
				if (!currentValid && value == count - 1)
				{
					conf.scale = current;
				}
				else
				{
					conf.scale = value + 1;
				}
				gui::SDLWindow::Ref().Recreate(conf);
				prefs::GlobalPrefs::Ref().Set("Scale", conf.scale);
			});
		}
		otherDrop(0, 100, "DEFAULT_LS_OPTIONS_RENDERER"_Ls(), { // * TODO-REDO_UI: better explanation
			"DEFAULT_LS_OPTIONS_RENDERER_DEFAULT"_Ls(),
			"DEFAULT_LS_OPTIONS_RENDERER_SOFTWARE"_Ls(),
			"DEFAULT_LS_OPTIONS_RENDERER_ACCELERATED"_Ls(),
		}, int(gui::SDLWindow::Ref().Config().rendererType), [](int value) {
			prefs::GlobalPrefs::Ref().Set("RendererType", value);
			auto conf = gui::SDLWindow::Ref().Config();
			conf.rendererType = gui::SDLWindow::Configuration::RendererType(value);
			gui::SDLWindow::Ref().Recreate(conf);
		});
		otherCheck(0, "DEFAULT_LS_OPTIONS_RESIZABLE"_Ls(), "DEFAULT_LS_OPTIONS_RESIZABLE_DESC"_Ls(), gui::SDLWindow::Ref().Config().resizable, [](bool value) {
			auto conf = gui::SDLWindow::Ref().Config();
			conf.resizable = value;
			gui::SDLWindow::Ref().Recreate(conf);
		});
		otherCheck(0, "DEFAULT_LS_OPTIONS_BLURRY"_Ls(), "DEFAULT_LS_OPTIONS_BLURRY_DESC"_Ls(), gui::SDLWindow::Ref().Config().blurry, [](bool value) {
			auto conf = gui::SDLWindow::Ref().Config();
			conf.blurry = value;
			gui::SDLWindow::Ref().Recreate(conf);
		});
		otherCheck(0, "DEFAULT_LS_OPTIONS_FULLSCREEN"_Ls(), "DEFAULT_LS_OPTIONS_FULLSCREEN_DESC"_Ls(), gui::SDLWindow::Ref().Config().fullscreen, [](bool value) {
			auto conf = gui::SDLWindow::Ref().Config();
			conf.fullscreen = value;
			gui::SDLWindow::Ref().Recreate(conf);
		});
		otherCheck(1, "DEFAULT_LS_OPTIONS_CHANGERES"_Ls(), "DEFAULT_LS_OPTIONS_CHANGERES_DESC"_Ls(), gui::SDLWindow::Ref().Config().borderless, [](bool value) {
			auto conf = gui::SDLWindow::Ref().Config();
			conf.borderless = value;
			gui::SDLWindow::Ref().Recreate(conf);
		});
		otherCheck(1, "DEFAULT_LS_OPTIONS_FORCEINTEGER"_Ls(), "DEFAULT_LS_OPTIONS_FORCEINTEGER_DESC"_Ls(), gui::SDLWindow::Ref().Config().integer, [](bool value) {
			auto conf = gui::SDLWindow::Ref().Config();
			conf.integer = value;
			gui::SDLWindow::Ref().Recreate(conf);
		});
		otherCheck(0, "DEFAULT_LS_OPTIONS_FASTQUIT"_Ls(), "DEFAULT_LS_OPTIONS_FASTQUIT_DESC"_Ls(), gui::SDLWindow::Ref().FastQuit(), [](bool value) {
			gui::SDLWindow::Ref().FastQuit(value);
			prefs::GlobalPrefs::Ref().Set("FastQuit", value);
		});
		otherCheck(0, "DEFAULT_LS_OPTIONS_SHOWAVATARS"_Ls(), "DEFAULT_LS_OPTIONS_SHOWAVATARS_DESC"_Ls(), prefs::GlobalPrefs::Ref().Get("ShowAvatars", true), [](bool value) { // * TODO: Load pref elsewhere.
			prefs::GlobalPrefs::Ref().Set("ShowAvatars", value);
		});
		otherCheck(0, "DEFAULT_LS_OPTIONS_MOMENTUMSCROLL"_Ls(), "DEFAULT_LS_OPTIONS_MOMENTUMSCROLL_DESC"_Ls(), gui::SDLWindow::Ref().MomentumScroll(), [](bool value) {
			gui::SDLWindow::Ref().MomentumScroll(value);
			prefs::GlobalPrefs::Ref().Set("MomentumScroll", value);
		});
		otherCheck(0, "DEFAULT_LS_OPTIONS_STICKYCAT"_Ls(), "DEFAULT_LS_OPTIONS_STICKYCAT_DESC"_Ls(), gm.StickyCategories(), [](bool value) {
			game::Game::Ref().StickyCategories(value);
			prefs::GlobalPrefs::Ref().Set("MouseClickRequired", value);
		});
		otherCheck(0, "DEFAULT_LS_OPTIONS_INCLUDEPRESS"_Ls(), "DEFAULT_LS_OPTIONS_INCLUDEPRESS_DESC"_Ls(), game::Game::Ref().IncludePressure(), [](bool value) {
			game::Game::Ref().IncludePressure(value);
			prefs::GlobalPrefs::Ref().Set("Simulation.IncludePressure", value);
		});
		otherCheck(0, "DEFAULT_LS_OPTIONS_PERFECTCIRC"_Ls(), "DEFAULT_LS_OPTIONS_PERFECTCIRC_DESC"_Ls(), gm.PerfectCircle(), [](bool value) {
			game::Game::Ref().PerfectCircle(value);
			prefs::GlobalPrefs::Ref().Set("PerfectCircleBrush", value);
		});
		otherDrop(0, 60, "DEFAULT_LS_OPTIONS_COLORSPACE"_Ls(), {
			"DEFAULT_LS_OPTIONS_COLORSPACE_SRGB"_Ls(),
			"DEFAULT_LS_OPTIONS_COLORSPACE_LINEAR"_Ls(),
			"DEFAULT_LS_OPTIONS_COLORSPACE_GAMMA22"_Ls(),
			"DEFAULT_LS_OPTIONS_COLORSPACE_GAMMA18"_Ls(),
		}, int(game::Game::Ref().DecoSpace()), [](int value) {
			game::Game::Ref().DecoSpace(game::SimDecoSpace(value));
			prefs::GlobalPrefs::Ref().Set("Simulation.DecoSpace", value);
		});
		otherButton(0, 90, "DEFAULT_LS_OPTIONS_DATAFOLDER"_Ls(), "DEFAULT_LS_OPTIONS_DATAFOLDER_DESC"_Ls(), []() {
			Platform::OpenDataFolder();
		});
		interorHeight += 4;

		spi->Size({ windowSize.x - 2, interorHeight });
	}
}
