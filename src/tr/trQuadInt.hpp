#pragma once

#include <array>
#include <cstdint>
#include "trQuadSizeT.hpp"
#include "trQuadMask.hpp"

namespace tr
{
	class QuadInt
	{
	public:
		                              QuadInt(const int32_t a);
		                              QuadInt(const int32_t a, const int32_t b, const int32_t c, const int32_t d);

#ifdef TR_SIMD
		                              QuadInt(const __m128i data);
#endif

		QuadInt&                      operator+=(const QuadInt& rhs);
		QuadInt&                      operator*=(const QuadInt& rhs);
		QuadInt&                      operator<<=(const int count);

		QuadInt                       operator+(const QuadInt& rhs) const;
		QuadInt                       operator*(const QuadInt& rhs) const;

		QuadInt                       operator|(const QuadInt& rhs) const;

		QuadSizeT                     toQuadSizeT() const;

		void                          write(int32_t* const address, const QuadMask& mask) const;

	private:
#ifdef TR_SIMD
		__m128i                       m_data;
#else
		std::array<int32_t, 4>        m_data;
#endif
	};
}