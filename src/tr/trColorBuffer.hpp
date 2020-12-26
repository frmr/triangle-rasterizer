#pragma once

#include "trBuffer.hpp"
#include "trColor.hpp"
#include "trQuadColor.hpp"
#include "trQuadInt.hpp"
#include <cstdint>

namespace tr
{
	class ColorBuffer : public Buffer<Color>
	{
	public:
		          ColorBuffer();
		          ColorBuffer(const size_t width, const size_t height);

		QuadColor getAt(const QuadInt& x, const QuadInt& y, const QuadMask& mask) const;
		QuadColor getAt(const QuadFloat& u, const QuadFloat& v, const QuadMask& mask) const;
		QuadColor getAt(QuadFloat u, QuadFloat v, const bool filter, const TextureWrappingMode textureWrappingMode, const QuadMask& mask) const;

	private:
		QuadInt   m_quadWidth; // Int rather than size_t because no SIMD multiply for vectors of 64-bit ints (see getAt())
		QuadInt   m_quadHeight;
		QuadFloat m_quadFloatWidth;
		QuadFloat m_quadFloatHeight;
	};
}