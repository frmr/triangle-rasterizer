#pragma once

#include "trError.hpp"
#include "trBuffer.hpp"
#include "trColor.hpp"
#include "trQuadColor.hpp"
#include "trQuadSizeT.hpp"
#include <cstdint>

namespace tr
{
	class ColorBuffer : public Buffer<Color>
	{
	public:
		ColorBuffer();
		ColorBuffer(const size_t width, const size_t height);

		QuadColor getAt(const QuadFloat& u, const QuadFloat& v) const;

	private:
		QuadInt   m_quadWidth; // Int rather than size_t because no SIMD multiply for vectors of 64-bit ints (see getAt())
		QuadFloat m_quadFloatWidth;
		QuadFloat m_quadFloatHeight;
		QuadSizeT m_quadDataPointer;
	};
}