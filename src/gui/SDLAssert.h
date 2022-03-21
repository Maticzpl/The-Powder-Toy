#pragma once

#include "common/Abort.h"

#include <SDL.h>

inline void SDLAssertTrue(SDL_bool res, const char *where)
{
	if (res != SDL_TRUE)
	{
		Abort(where, SDL_GetError());
	}
}

inline void SDLAssertZero(int res, const char *where)
{
	if (res)
	{
		Abort(where, SDL_GetError());
	}
}

template<class Type>
inline Type *SDLAssertPtr(Type *ptr, const char *where)
{
	if (!ptr)
	{
		Abort(where, SDL_GetError());
	}
	return ptr;
}

#define SDLASSERTTRUE(x) SDLAssertTrue(x, __FILE__ ":" MTOS(__LINE__) ": " #x " failed: ")
#define SDLASSERTZERO(x) SDLAssertZero(x, __FILE__ ":" MTOS(__LINE__) ": " #x " failed: ")
#define SDLASSERTPTR(x) SDLAssertPtr(x, __FILE__ ":" MTOS(__LINE__) ": " #x " failed: ")
