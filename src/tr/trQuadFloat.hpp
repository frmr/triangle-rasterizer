#pragma once
#include "trQuadMask.hpp"
#include <array>

namespace tr
{
	class QuadInt;

	class QuadFloat
	{
	public:
		                     QuadFloat(const float a);
		                     QuadFloat(const float a, const float b, const float c, const float d);
		                     QuadFloat(const float* const pointer);
		                     QuadFloat(const float* const pointer, const QuadMask& mask);

#ifdef TR_SIMD
		                     QuadFloat(const __m128 data);
#endif

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

		QuadFloat            min(const QuadFloat& rhs) const;
		QuadFloat            max(const QuadFloat& rhs) const;

		QuadMask             castToMask() const;

		QuadFloat            abs() const;
		QuadFloat            floor() const;
		QuadFloat            round() const;

		QuadInt              convertToQuadInt() const;

		void                 write(float* const pointer, const QuadMask& mask) const;

	private:
#ifdef TR_SIMD
		__m128               m_data;
#else
		std::array<float, 4> m_data;
#endif
	};
}