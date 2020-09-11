#pragma once

#include <array>
#include <immintrin.h>

namespace tr
{
	class QuadSizeT
	{
	public:
		                      QuadSizeT(const size_t a);
		                      QuadSizeT(const size_t a, const size_t b, const size_t c, const size_t d);

#ifdef TR_SIMD
		                      QuadSizeT(const __m256i data);
#endif

		QuadSizeT&            operator+=(const QuadSizeT& rhs);
		QuadSizeT             operator+(const QuadSizeT& rhs) const;

		std::array<size_t, 4> toArray() const;

	private:
#ifdef TR_SIMD
		__m256i               m_data;
#else
		std::array<size_t, 4> m_data;
#endif
	};
}