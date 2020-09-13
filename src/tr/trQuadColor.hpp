#pragma once

#include "trColor.hpp"
#include "trQuadFloat.hpp"
#include "trQuadSizeT.hpp"
#include <array>

namespace tr
{
	class QuadColor
	{
	public:
		          QuadColor(const Color* const baseAddress, const QuadInt& offsets, const QuadMask& mask);

		void      write(Color* const pointer, const QuadMask& mask) const;

	private:
		QuadFloat m_r;
		QuadFloat m_g;
		QuadFloat m_b;
		QuadFloat m_a;
	};
}