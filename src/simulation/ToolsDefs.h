#pragma once

#include "Config.h"

#define DRAW_FUNC_ARGS Simulation *sim, Particle *cpart, gui::Point pos, gui::Point origin, float strength
#define DRAW_FUNC_SUBCALL_ARGS sim, cpart, pos, origin, strength

#define FILL_FUNC_ARGS Simulation *sim, gui::Point pos, float strength
#define FILL_FUNC_SUBCALL_ARGS sim, pos, strength

#define LINE_FUNC_ARGS Simulation *sim, gui::Point from, gui::Point to, float strength
#define LINE_FUNC_SUBCALL_ARGS sim, from, to, strength

#define CLICK_FUNC_ARGS Simulation *sim, gui::Point pos, float strength
#define CLICK_FUNC_SUBCALL_ARGS sim, pos, strength

#define DRAWBUTTON_FUNC_ARGS const activities::game::ToolButton *toolButton
#define DRAWBUTTON_FUNC_SUBCALL_ARGS toolButton
