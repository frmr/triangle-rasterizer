#pragma once

#include "trColor.hpp"
#include "trQuadFloat.hpp"
#include "trQuadSizeT.hpp"
#include <array>

namespace tr
{
	constexpr std::array<float, 256> generateConversionTable();

	class QuadColor
	{
	public:
		          QuadColor(const QuadSizeT& pointers);

		void      write(Color* const pointer, const QuadMask& mask) const;

	private:
		QuadFloat m_r;
		QuadFloat m_g;
		QuadFloat m_b;
		QuadFloat m_a;
	};
}