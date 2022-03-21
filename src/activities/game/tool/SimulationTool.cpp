#include "SimulationTool.h"

#include "activities/game/Game.h"
#include "simulation/Simulation.h"
#include "simulation/ToolClasses.h"
#include "graphics/Pix.h"

namespace activities::game::tool
{
	SimulationTool::SimulationTool(int newSimple, String newDescription) : simple(newSimple)
	{
		auto &tl = Game::Ref().SimulationTools()[simple];
		Name(tl.Name);
		Description(newDescription);
		Color({ PixR(tl.Colour), PixG(tl.Colour), PixB(tl.Colour), 255 });
		MenuVisible(true);
		MenuSection(tl.MenuSection);
		UseOffsetLists(tl.UseOffsetLists);
		MenuPrecedence(simple);
		if (tl.Line)
		{
			Application(applicationLine);
		}
		if (tl.Click)
		{
			Application(applicationClick);
		}
		if (tl.DrawButton)
		{
			CanDrawButton(true);
		}
	}

	void SimulationTool::Draw(gui::Point pos, gui::Point origin) const
	{
		auto &game = Game::Ref();
		auto *sim = game.GetSimulation();
		auto &tl = game.SimulationTools()[simple];
		auto strength = 1.0f; // TODO-REDO_UI
		if (tl.Draw)
		{
			auto x = pos.x;
			auto y = pos.y;
			Particle *cpart = nullptr;
			if (sim->pmap[y][x])
			{
				cpart = &(sim->parts[ID(sim->pmap[y][x])]);
			}
			else if (sim->photons[y][x])
			{
				cpart = &(sim->parts[ID(sim->photons[y][x])]);
			}
			tl.Draw(sim, cpart, pos, origin, strength);
		}
	}

	void SimulationTool::Fill(gui::Point pos) const
	{
		auto &game = Game::Ref();
		auto *sim = game.GetSimulation();
		auto &tl = game.SimulationTools()[simple];
		auto strength = 1.0f; // TODO-REDO_UI
		if (tl.Fill)
		{
			tl.Fill(sim, pos, strength);
		}
	}

	void SimulationTool::Line(gui::Point from, gui::Point to) const
	{
		auto &game = Game::Ref();
		auto *sim = game.GetSimulation();
		auto &tl = game.SimulationTools()[simple];
		auto strength = 1.0f; // TODO-REDO_UI
		if (tl.Line)
		{
			tl.Line(sim, from, to, strength);
		}
	}

	void SimulationTool::Click(gui::Point pos) const
	{
		auto &game = Game::Ref();
		auto *sim = game.GetSimulation();
		auto &tl = game.SimulationTools()[simple];
		auto strength = 1.0f; // TODO-REDO_UI
		if (tl.Click)
		{
			tl.Click(sim, pos, strength);
		}
	}

	bool SimulationTool::Select()
	{
		auto &game = Game::Ref();
		auto &tl = game.SimulationTools()[simple];
		if (tl.Select)
		{
			return tl.Select();
		}
		return false;
	}

	void SimulationTool::DrawButton(const ToolButton *toolButton) const
	{
		auto &game = Game::Ref();
		auto &tl = game.SimulationTools()[simple];
		tl.DrawButton(toolButton);
	}
}
