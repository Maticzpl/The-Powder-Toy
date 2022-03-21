#pragma once
#include "Config.h"

#include <iostream>
#include <cstdlib>

inline void Abort(const char *where, const char *what)
{
	std::cerr << where << what << std::endl;
	abort();
}
