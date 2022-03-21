#include "Login.h"

#include "activities/game/Game.h"
#include "activities/dialog/Error.h"
#include "gui/Appearance.h"
#include "gui/Button.h"
#include "gui/IconTextBox.h"
#include "gui/Label.h"
#include "gui/Static.h"
#include "gui/Separator.h"
#include "gui/SDLWindow.h"
#include "gui/Icons.h"
#include "graphics/FontData.h"
#include "backend/Backend.h"
#include "language/Language.h"

#include <SDL.h>

namespace activities::login
{
	constexpr auto parentSize = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto windowSize = gui::Point{ 200, 94 };

	Login::Login()
	{
		auto *title = EmplaceChild<gui::Static>().get();
		title->Position({ 6, 5 });
		title->Size({ windowSize.x - 20, 14 });
		title->Text("DEFAULT_LS_LOGIN_TITLE"_Ls());
		title->TextColor(gui::Appearance::informationTitle);

		auto *sep = EmplaceChild<gui::Separator>().get();
		sep->Position({ 1, 22 });
		sep->Size({ windowSize.x - 2, 1 });

		auto *signInButton = EmplaceChild<gui::Button>().get();
		signInButton->Text(String::Build(gui::CommonColorString(gui::commonYellow), "DEFAULT_LS_LOGIN_SIGNIN"_Ls()));
		signInButton->Click([this]() {
			auto &username = usernameBox->Text();
			auto &password = passwordBox->Text();
			if (!username.size() || !password.size())
			{
				return;
			}
			if (username.Contains("@"))
			{
				gui::SDLWindow::Ref().EmplaceBack<dialog::Error>("DEFAULT_LS_LOGIN_ERROR"_Ls(), "DEFAULT_LS_LOGIN_STATUS_EMAIL"_Ls());
			}
			else
			{
				// // * TODO-REDO_UI: Make this async once Client gets refactored.
				// User user(0, "");
				// Client::Ref().Login(username.ToUtf8(), password.ToUtf8(), user);
				// if (user.UserID)
				// {
				// 	Client::Ref().SetAuthUser(user);
				// 	game::Game::Ref().UpdateLoginControls();
				// 	Quit();
				// }
				// else
				// {
				// 	statusText = Client::Ref().GetLastError();
				// }
			}
		});
		Okay([signInButton = signInButton]() {
			signInButton->TriggerClick();
		});

		auto *signOutButton = EmplaceChild<gui::Button>().get();
		signOutButton->Text("DEFAULT_LS_LOGIN_SIGNOUT"_Ls());
		signOutButton->Click([this]() {
			backend::Backend::Ref().User(backend::UserInfo{});
			game::Game::Ref().UpdateLoginControls();
			Quit();
		});

		usernameBox = EmplaceChild<gui::IconTextBox>().get();
		usernameBox->IconString(
			gui::ColorString({ 0x20, 0x40, 0x80 }) + gui::IconString(gui::Icons::userBody   , { 1, -1 }) + gui::StepBackString() +
			gui::ColorString({ 0xFF, 0xFF, 0xFF }) + gui::IconString(gui::Icons::userOutline, { 1, -1 })
		);
		usernameBox->IconPadding(16);
		usernameBox->PlaceHolder(String::Build("[", "DEFAULT_LS_LOGIN_USERNAME"_Ls(), "]"));
		usernameBox->Position({ 8, 31 });
		usernameBox->Size({ windowSize.x - 16, 17 });

		passwordBox = EmplaceChild<gui::IconTextBox>().get();
		passwordBox->IconString(
			gui::ColorString({ 0xA0, 0x90, 0x20 }) + gui::IconString(gui::Icons::passBody   , { 1, -1 }) + gui::StepBackString() +
			gui::ColorString({ 0xFF, 0xFF, 0xFF }) + gui::IconString(gui::Icons::passOutline, { 1, -1 })
		);
		passwordBox->IconPadding(16);
		passwordBox->PlaceHolder(String::Build("[", "DEFAULT_LS_LOGIN_PASSWORD"_Ls(), "]"));
		passwordBox->Position({ 8, 52 });
		passwordBox->Size({ windowSize.x - 16, 17 });
		passwordBox->AllowCopy(false);
		passwordBox->Format([](const String &unformatted) -> String {
			return String(unformatted.size(), gui::Icons::maskChar);
		});

		usernameBox->Focus();
		if (backend::Backend::Ref().User().detailLevel)
		{
			usernameBox->Text(backend::Backend::Ref().User().name);
			usernameBox->SelectAll();
		}

		signOutButton->Visible(false);
		if (backend::Backend::Ref().User().detailLevel)
		{
			signOutButton->Visible(true);
			auto floorW2 = windowSize.x / 2;
			auto ceilW2 = windowSize.x - floorW2;
			signOutButton->Position({ 0, windowSize.y - 17 });
			signOutButton->Size({ ceilW2, 17 });
			signInButton->Position({ ceilW2 - 1, windowSize.y - 17 });
			signInButton->Size({ floorW2 + 1, 17 });
		}
		else
		{
			signInButton->Position({ 0, windowSize.y - 17 });
			signInButton->Size({ windowSize.x, 17 });
		}

		Size(windowSize);
		Position((parentSize - windowSize) / 2);
	}

	bool Login::KeyPress(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt)
	{
		switch (sym)
		{
		case SDLK_TAB:
			if (ChildWithFocusDeep() == usernameBox)
			{
				passwordBox->Focus();
			}
			else
			{
				usernameBox->Focus();
			}
			return true;

		default:
			break;
		}
		return PopupWindow::KeyPress(sym, scan, repeat, shift, ctrl, alt);
	}
}
