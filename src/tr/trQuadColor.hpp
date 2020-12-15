#pragma once

#include "trColor.hpp"
#include "trQuadFloat.hpp"
#include "trQuadSizeT.hpp"
#include "trQuadVec4.hpp"
#include <array>

namespace tr
{
	class QuadColor
	{
	public:
		          QuadColor(const QuadFloat& r, const QuadFloat& g, const QuadFloat& b, const QuadFloat& a);
		          QuadColor(const Color* const baseAddress, const QuadInt& offsets, const QuadMask& mask);
		          QuadColor(const QuadVec4& vec);

		void      write(Color* const pointer, const QuadMask& mask) const;

		QuadVec4  toVector() const;

	private:
		QuadFloat m_r;
		QuadFloat m_g;
		QuadFloat m_b;
		QuadFloat m_a;
	};
}