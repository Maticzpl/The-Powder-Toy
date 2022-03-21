#include "ElementTool.h"

#include "activities/game/Game.h"
#include "simulation/Simulation.h"
#include "simulation/ElementClasses.h"
#include "graphics/Pix.h"
#include "graphics/CommonShapes.h"

namespace activities::game::tool
{
	ElementTool::ElementTool(int newElem, String newDescription) : elem(newElem)
	{
		auto &el = Game::Ref().GetSimulation()->elements[TYP(elem)];
		Name(el.Name);
		Description(newDescription);
		Color({ PixR(el.Colour), PixG(el.Colour), PixB(el.Colour), 255 });
		MenuVisible(el.MenuVisible);
		MenuSection(el.MenuSection);
		MenuPrecedence(elem);
		UseOffsetLists(true);
		AllowReplace(true);
		CanGenerateTexture(TYP(elem) == PT_NONE);
	}

	void ElementTool::Draw(gui::Point pos, gui::Point origin) const
	{
		auto *sim = Game::Ref().GetSimulation();
		sim->CreateParts(pos.x, pos.y, 0, 0, elem, 0);
	}

	void ElementTool::Fill(gui::Point pos) const
	{
		auto *sim = Game::Ref().GetSimulation();
		sim->FloodParts(pos.x, pos.y, elem, -1, 0);
	}

	void ElementTool::GenerateTexture(gui::Image &image) const
	{
		auto drawPixel = [&image](int x, int y, int c) {
			image(x, y) = c;
		};
		switch (TYP(elem))
		{
		case PT_NONE:
			for (auto y = 0; y < image.Size().y; ++y)
			{
				for (auto x = 0; x < image.Size().x; ++x)
				{
					image(x, y) = UINT32_C(0xFF000000);
				}
			}
			RedCross(9, 3, drawPixel);
			break;
		}
	}
}
