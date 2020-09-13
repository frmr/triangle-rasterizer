#pragma once

#include <array>
#include <immintrin.h>

namespace tr
{
	class QuadMask
	{
	public:
#ifdef TR_SIMD
		                    QuadMask(const __m128 data);
#else
		                    QuadMask(const bool a);
		                    QuadMask(const bool a, const bool b, const bool c, const bool d);
#endif



		QuadMask&           operator&=(const QuadMask& rhs);
		QuadMask            operator&(const QuadMask& rhs) const;
		QuadMask            operator|(const QuadMask& rhs) const;

		bool                moveMask() const;

		QuadMask            inverse() const;

#ifdef TR_SIMD
		__m128              getData() const;
#else
		bool                get(const size_t index) const;
#endif

	private:
#ifdef TR_SIMD
		__m128              m_data;
#else
		std::array<bool, 4> m_data;
#endif
	};
}