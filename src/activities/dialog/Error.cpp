#include "Error.h"

#include "gui/Appearance.h"
#include "gui/Button.h"
#include "gui/Static.h"
#include "gui/Label.h"
#include "gui/Separator.h"
#include "graphics/FontData.h"
#include "language/Language.h"

namespace activities::dialog
{
	Error::Error(String titleText, String bodyText) : Message(titleText, bodyText)
	{
		title->TextColor(gui::Appearance::errorTitle);
	}
}
