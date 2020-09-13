#include "trQuadSizeT.hpp"

tr::QuadSizeT::QuadSizeT(const size_t a) :
#ifdef TR_SIMD
	m_data(_mm256_setr_epi64x(a, a, a, a))
#else
	m_data{ a, a, a, a }
#endif
{
}

tr::QuadSizeT::QuadSizeT(const size_t a, const size_t b, const size_t c, const size_t d) :
#ifdef TR_SIMD
	m_data(_mm256_setr_epi64x(a, b, c, d))
#else
	m_data{ a, b, c, d }
#endif
{
}

#ifdef TR_SIMD
tr::QuadSizeT::QuadSizeT(const __m256i data) :
	m_data(data)
{
}
#endif

tr::QuadSizeT& tr::QuadSizeT::operator+=(const QuadSizeT& rhs)
{
#ifdef TR_SIMD
	m_data = _mm256_add_epi64(m_data, rhs.m_data);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] += rhs.m_data[i];
	}
#endif

	return *this;
}

tr::QuadSizeT tr::QuadSizeT::operator+(const QuadSizeT& rhs) const
{
#ifdef TR_SIMD
	return QuadSizeT(_mm256_add_epi64(m_data, rhs.m_data));
#else
	QuadSizeT result = *this;

	result += rhs;

	return result;
#endif
}
