#pragma once

#include "trQuadMask.hpp"
#include "trQuadInt.hpp"
#include <array>

namespace tr
{
	class QuadFloat
	{
	public:
		                     QuadFloat(const float a);
		                     QuadFloat(const float a, const float b, const float c, const float d);
		                     QuadFloat(const float* const pointer);

		QuadFloat&           operator+=(const QuadFloat& rhs);
		QuadFloat&           operator-=(const QuadFloat& rhs);
		QuadFloat&           operator*=(const QuadFloat& rhs);
		QuadFloat&           operator/=(const QuadFloat& rhs);
		QuadFloat            operator+(const QuadFloat& rhs) const;
		QuadFloat            operator-(const QuadFloat& rhs) const;
		QuadFloat            operator*(const QuadFloat& rhs) const;
		QuadFloat            operator/(const QuadFloat& rhs) const;

		QuadFloat            operator&(const QuadFloat& rhs) const;
		QuadFloat            operator|(const QuadFloat& rhs) const;

		QuadMask             greaterThan(const QuadFloat& rhs) const;
		QuadMask             lessThan(const QuadFloat& rhs) const;

		QuadMask             castToMask() const;

		QuadFloat            abs() const;
		QuadFloat            floor() const;
		QuadFloat            round() const;

		QuadInt              convertToQuadInt() const;

		void                 write(float* const pointer) const;

#ifdef TR_SIMD
	private:
		                     QuadFloat(const __m128 data);
#endif

	private:
#ifdef TR_SIMD
		__m128               m_data;
#else
		std::array<float, 4> m_data;
#endif
	};
}