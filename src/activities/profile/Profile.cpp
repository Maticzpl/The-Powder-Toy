#include "Profile.h"

#include "common/Platform.h"
#include "gui/Appearance.h"
#include "gui/Button.h"
#include "gui/ImageButton.h"
#include "gui/Label.h"
#include "gui/TextBox.h"
#include "gui/ScrollPanel.h"
#include "gui/Separator.h"
#include "language/Language.h"

namespace activities::profile
{
	constexpr auto parentSize = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto windowSize = gui::Point{ 236, 301 };

	Profile::Profile(bool editable)
	{
		Size(windowSize);
		Position((parentSize - windowSize) / 2);

		{
			auto title = EmplaceChild<gui::Static>();
			title->Position({ 6, 5 });
			title->Size({ windowSize.x - 20, 14 });
			title->Text(editable ? "DEFAULT_LS_PROFILE_EDITTITLE"_Ls() : "DEFAULT_LS_PROFILE_VIEWTITLE"_Ls());
			title->TextColor(gui::Appearance::informationTitle);
			auto sep = EmplaceChild<gui::Separator>();
			sep->Position({ 1, 22 });
			sep->Size({ windowSize.x - 2, 1 });
			auto ok = EmplaceChild<gui::Button>();
			if (editable)
			{
				auto floorW2 = windowSize.x / 2;
				auto ceilW2 = windowSize.x - floorW2;
				ok->Text(String::Build("\bo", "DEFAULT_LS_PROFILE_SAVE"_Ls()));
				ok->Position({ ceilW2 - 1, windowSize.y - 17 });
				ok->Size({ floorW2 + 1, 17 });
				ok->Click([this]() {
					// * TODO-REDO_UI: save
					Quit();
				});
				auto cancel = EmplaceChild<gui::Button>();
				cancel->Text("DEFAULT_LS_PROFILE_CANCEL"_Ls());
				cancel->Position({ 0, windowSize.y - 17 });
				cancel->Size({ ceilW2, 17 });
				cancel->Click([this]() {
					Quit();
				});
				Cancel([cancel]() {
					cancel->TriggerClick();
				});
				Okay([ok]() {
					ok->TriggerClick();
				});
			}
			else
			{
				ok->Text(String::Build("\bo", "DEFAULT_LS_PROFILE_OK"_Ls()));
				ok->Position({ 0, windowSize.y - 16 });
				ok->Size({ windowSize.x, 16 });
				ok->Click([this]() {
					// * TODO-REDO_UI: save data
					Quit();
				});
				Cancel([ok]() {
					ok->TriggerClick();
				});
				Okay([ok]() {
					ok->TriggerClick();
				});
			}
		}

		auto ailNextY = 31;
		auto addInfoLine = [this, &ailNextY](int labelSize, String labelText, String infoText) {
			auto *label = EmplaceChild<gui::Static>().get();
			label->Text(labelText);
			label->Position({ 8, ailNextY });
			label->Size({ labelSize, 15 });
			label->TextColor(gui::Appearance::commonLightGray);
			auto *info = EmplaceChild<gui::Label>().get();
			info->Text(infoText);
			info->Position({ 8 + labelSize, ailNextY });
			info->Size({ windowSize.x - 60 - labelSize, 15 });
			ailNextY += 15;
			return info;
		};

		// * TODO-REDO_UI: load data
		addInfoLine(50, "DEFAULT_LS_PROFILE_USERNAME"_Ls(), "example");
		addInfoLine(50, "DEFAULT_LS_PROFILE_LOCATION"_Ls(), "Example, Example");
		websiteTextBox = addInfoLine(50, "Website:", "https://example.com/");
		addInfoLine(75, "DEFAULT_LS_PROFILE_SAVECOUNT"_Ls(), "42");
		addInfoLine(75, "DEFAULT_LS_PROFILE_AVGSCORE"_Ls(), "3.14");
		addInfoLine(75, "DEFAULT_LS_PROFILE_HIGHSCORE"_Ls(), "1337");

		if (editable)
		{
			auto text = websiteTextBox->Text();
			websiteTextBox->DrawBody(true);
			websiteTextBox->DrawBorder(true);
			websiteTextBox->DrawCursor(true);
			websiteTextBox->Editable(true);
			websiteTextBox->TextRect(websiteTextBox->TextRect().Offset({ 0, -1 }).Grow({ 0, 2 }));
			websiteTextBox->Format(nullptr);
			websiteTextBox->Text(text);
		}

		{
			auto *label = EmplaceChild<gui::Static>().get();
			label->Text("DEFAULT_LS_PROFILE_BIOGRAPHY"_Ls());
			label->Position({ 8, ailNextY });
			label->Size({ 50, 15 });
			label->TextColor(gui::Appearance::commonLightGray);
			auto *button = EmplaceChild<gui::Button>().get();
			button->Text("DEFAULT_LS_PROFILE_EDITDETAILS"_Ls());
			button->Size({ 100, 15 });
			button->Position({ windowSize.x - 8 - button->Size().x, ailNextY });
			button->Click([]() {
				Platform::OpenURI(SCHEME SERVER "/Profile.html");
			});
			ailNextY += 15;
		}

		{
			auto *image = EmplaceChild<gui::ImageButton>().get();
			image->Size({ 40, 40 });
			image->Position({ windowSize.x - 8 - image->Size().x, 31 });
			auto *button = EmplaceChild<gui::Button>().get();
			button->Text("DEFAULT_LS_PROFILE_CHANGEAVATAR"_Ls());
			button->Size({ 40, 15 });
			button->Position({ windowSize.x - 8 - button->Size().x, 75 });
			button->Click([]() {
				Platform::OpenURI(SCHEME SERVER "/Profile/Avatar.html");
			});
		}

		if (editable)
		{
			ailNextY += 4;
			biographyTextBox = EmplaceChild<gui::TextBox>().get();
			biographyTextBox->Multiline(true);
			biographyTextBox->Position({ 8, ailNextY });
			biographyTextBox->Size({ windowSize.x - 16, windowSize.y - ailNextY - 24 });
			biographyTextBox->TextRect(biographyTextBox->TextRect().Grow({ -6, 0 }));
		}
	}
}
