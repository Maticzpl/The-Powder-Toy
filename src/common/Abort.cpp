#include "Abort.h"

#include <iostream>
#include <cstdlib>

#if defined(AND)
# include <SDL.h>

void Abort(const char *where, const char *what)
{
	SDL_Log("%s%s\n", where, what);
	abort();
}
#else
void Abort(const char *where, const char *what)
{
	std::cerr << where << what << std::endl;
	abort();
}
#endif
