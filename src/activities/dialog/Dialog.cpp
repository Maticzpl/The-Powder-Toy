#include "Dialog.h"

#include "gui/Static.h"
#include "gui/Label.h"
#include "gui/Separator.h"
#include "graphics/FontData.h"

namespace activities::dialog
{
	constexpr auto parentSize   = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto maxBodyLines = 8;

	Dialog::Dialog(String titleText, String bodyText, gui::Point windowSize)
	{
		title = EmplaceChild<gui::Static>().get();
		title->Position({ 6, 5 });
		title->Size({ windowSize.x - 20, 14 });
		title->Text(titleText);

		auto *sep = EmplaceChild<gui::Separator>().get();
		sep->Position({ 1, 22 });
		sep->Size({ windowSize.x - 2, 1 });

		auto *body = EmplaceChild<gui::Label>().get();
		body->Position({ 5, 27 });
		body->Multiline(true);

		targetSize = windowSize;
		if (bodyText.size())
		{
			body->Text(bodyText);
			// * Update size first so our next reading of Lines will be accurate.
			body->Size({ targetSize.x - 10, 17 });
			auto lines = body->Lines();
			if (lines > maxBodyLines)
			{
				lines = maxBodyLines;
			}
			body->Size({ targetSize.x - 10, 5 + FONT_H * lines });
			targetSize.y += 6 + FONT_H * lines;
		}
		Size(targetSize);
		Position((parentSize - targetSize) / 2);
	}
}
