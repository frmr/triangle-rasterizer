#pragma once

#include <cstdint>

namespace tr
{
	constexpr uint8_t leftBitMask   = 1;
	constexpr uint8_t rightBitMask  = 2;
	constexpr uint8_t topBitMask    = 4;
	constexpr uint8_t bottomBitMask = 8;
	constexpr uint8_t nearBitMask   = 16;
	constexpr uint8_t farBitMask    = 32;
	constexpr uint8_t wBitMask      = 64;
}