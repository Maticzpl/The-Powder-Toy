#pragma once

#include "common/String.h"

#include <cstdint>

struct BuiltinGOL
{
	String identifier;
	String name;
	int oldtype;
	int ruleset;
	uint32_t colour, colour2;
	int goltype;
};
