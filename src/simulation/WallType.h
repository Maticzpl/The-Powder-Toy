#ifndef WALLTYPE_H_
#define WALLTYPE_H_

#include "common/String.h"

#include <cstdint>

struct wall_type
{
	uint32_t colour;
	uint32_t eglow; // if emap set, add this to fire glow
	enum DrawStyle
	{
		SOLIDDOT,
		SPARSEDOT,
		HEXAGONAL,
		POWERED0,
		POWERED1,
		DIAGONAL,
		SPECIAL,
	};
	DrawStyle drawstyle;
	String name;
	ByteString identifier;
	String descs;
};

#endif
