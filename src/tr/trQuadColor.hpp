#pragma once

#include "trColor.hpp"
#include "trQuadFloat.hpp"
#include <array>

namespace tr
{
	class QuadColor
	{
	public:
		           QuadColor(const QuadFloat& r, const QuadFloat& g, const QuadFloat& b, const QuadFloat& a);
		           QuadColor(const Color* const baseAddress, const QuadInt& offsets, const QuadMask& mask);

		void       write(Color* const pointer, const QuadMask& mask) const;

		QuadColor& operator+=(const QuadColor& rhs);
		QuadColor& operator*=(const QuadFloat& rhs);

		QuadColor  operator+(const QuadColor& rhs) const;
		QuadColor  operator*(const QuadFloat& rhs) const;

	private:
		QuadFloat  m_r;
		QuadFloat  m_g;
		QuadFloat  m_b;
		QuadFloat  m_a;
	};
}