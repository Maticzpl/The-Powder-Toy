#include "Message.h"

#include "gui/Appearance.h"
#include "gui/Button.h"
#include "gui/Static.h"
#include "gui/Label.h"
#include "gui/Separator.h"
#include "graphics/FontData.h"
#include "language/Language.h"

namespace activities::dialog
{
	constexpr auto windowSize = gui::Point{ 200, 49 };

	Message::Message(String titleText, String bodyText) : Dialog(titleText, bodyText, windowSize)
	{
		title->TextColor(gui::Appearance::informationTitle);

		auto *okButton = EmplaceChild<gui::Button>().get();
		okButton->Text(String::Build("\bo", "DEFAULT_LS_MESSAGE_OK"_Ls()));
		okButton->Click([this]() {
			Quit();
			if (finish)
			{
				finish();
			}
		});
		Okay([okButton]() {
			okButton->TriggerClick();
		});

		okButton->Position({ 0, targetSize.y - 17 });
		okButton->Size({ targetSize.x, 17 });
	}
}
