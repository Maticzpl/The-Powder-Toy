#pragma once

#include <cstdint>

namespace gui
{
	namespace Alignment
	{
		constexpr uint32_t horizBits   = UINT32_C(0x00000003);
		constexpr uint32_t horizLeft   = UINT32_C(0x00000000);
		constexpr uint32_t horizCenter = UINT32_C(0x00000001);
		constexpr uint32_t horizRight  = UINT32_C(0x00000002);

		constexpr uint32_t vertBits    = UINT32_C(0x0000000C);
		constexpr uint32_t vertTop     = UINT32_C(0x00000000);
		constexpr uint32_t vertCenter  = UINT32_C(0x00000004);
		constexpr uint32_t vertBottom  = UINT32_C(0x00000008);
	};
}
