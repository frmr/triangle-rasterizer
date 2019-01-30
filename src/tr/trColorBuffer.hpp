#pragma once

#include "trBuffer.hpp"
#include "trColor.hpp"

namespace tr
{
	class ColorBuffer : public Buffer<Color>
	{
	public:
		using Buffer<Color>::Buffer;

		ColorBuffer(const std::string& filename);
	};
}