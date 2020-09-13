#include "trQuadMask.hpp"

#ifdef TR_SIMD
tr::QuadMask::QuadMask(const __m128 data) :
	m_data(data)
{
}
#else
tr::QuadMask::QuadMask(const bool a) :
	m_data{ a, a, a, a }
{
}

tr::QuadMask::QuadMask(const bool a, const bool b, const bool c, const bool d) :
	m_data{ a, b, c, d }
{
}
#endif

tr::QuadMask& tr::QuadMask::operator&=(const QuadMask& rhs)
{
#ifdef TR_SIMD
	m_data = _mm_and_ps(m_data, rhs.m_data);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] = m_data[i] && rhs.m_data[i];
	}
#endif

	return *this;
}

tr::QuadMask tr::QuadMask::operator&(const QuadMask& rhs) const
{
#ifdef TR_SIMD
	return QuadMask(_mm_and_ps(m_data, rhs.m_data));
#else
	return QuadMask(
		m_data[0] && rhs.m_data[0],
		m_data[1] && rhs.m_data[1],
		m_data[2] && rhs.m_data[2],
		m_data[3] && rhs.m_data[3]
	);
#endif
}

tr::QuadMask tr::QuadMask::operator|(const QuadMask& rhs) const
{
#ifdef TR_SIMD
	return QuadMask(_mm_or_ps(m_data, rhs.m_data));
#else
	return QuadMask(
		m_data[0] || rhs.m_data[0],
		m_data[1] || rhs.m_data[1],
		m_data[2] || rhs.m_data[2],
		m_data[3] || rhs.m_data[3]
	);
#endif
}

#ifdef TR_SIMD
__m128 tr::QuadMask::getData() const
{
	return m_data;
}
#else
bool tr::QuadMask::get(const size_t index) const
{
	return m_data[index];
}
#endif

bool tr::QuadMask::moveMask() const
{
#if TR_SIMD
	return _mm_movemask_ps(m_data) > 0;
#else
	return m_data[0] || m_data[1] || m_data[2] || m_data[3];
#endif
}

tr::QuadMask tr::QuadMask::inverse() const
{
#ifdef TR_SIMD
	static const __m128 allOnes = _mm_castsi128_ps(_mm_set1_epi32(-1));

	return _mm_xor_ps(m_data, allOnes);
#else
	return QuadMask(
		!m_data[0],
		!m_data[1],
		!m_data[2],
		!m_data[3]
	);
#endif
}
