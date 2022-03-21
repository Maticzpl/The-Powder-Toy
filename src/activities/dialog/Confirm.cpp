#include "Confirm.h"

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

	Confirm::Confirm(String titleText, String bodyText) : Dialog(titleText, bodyText, windowSize)
	{
		title->TextColor(gui::Appearance::informationTitle);

		auto *confirmButton = EmplaceChild<gui::Button>().get();
		confirmButton->Text(String::Build("\bo", "DEFAULT_LS_CONFIRM_CONFIRM"_Ls()));
		confirmButton->Click([this]() {
			Quit();
			if (finish)
			{
				finish(true);
			}
		});
		Okay([confirmButton]() {
			confirmButton->TriggerClick();
		});

		auto *cancelButton = EmplaceChild<gui::Button>().get();
		cancelButton->Text("DEFAULT_LS_CONFIRM_CANCEL"_Ls());
		cancelButton->Click([this]() {
			Quit();
			if (finish)
			{
				finish(false);
			}
		});
		Cancel([cancelButton]() {
			cancelButton->TriggerClick();
		});

		auto floorW2 = targetSize.x / 2;
		auto ceilW2 = targetSize.x - floorW2;
		cancelButton->Position({ 0, targetSize.y - 17 });
		cancelButton->Size({ ceilW2, 17 });
		confirmButton->Position({ ceilW2 - 1, targetSize.y - 17 });
		confirmButton->Size({ floorW2 + 1, 17 });
	}
}
