#include "trQuadInt.hpp"

tr::QuadInt::QuadInt(const int32_t a) :
#ifdef TR_SIMD
	m_data(_mm_set1_epi32(a))
#else
	m_data{ a, a, a, a }
#endif
{
}

tr::QuadInt::QuadInt(const int32_t a, const int32_t b, const int32_t c, const int32_t d) :
#ifdef TR_SIMD
	m_data(_mm_setr_epi32(a, b, c, d))
#else
	m_data{ a, b, c, d }
#endif
{
}

#ifdef TR_SIMD
tr::QuadInt::QuadInt(const __m128i data) :
	m_data(data)
{
}
#endif

tr::QuadInt& tr::QuadInt::operator+=(const QuadInt& rhs)
{
#ifdef TR_SIMD
	m_data = _mm_add_epi32(m_data, rhs.m_data);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] += rhs.m_data[i];
	}
#endif

	return *this;
}

tr::QuadInt& tr::QuadInt::operator*=(const QuadInt& rhs)
{
#ifdef TR_SIMD
	m_data = _mm_mul_epi32(m_data, rhs.m_data);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] *= rhs.m_data[i];
	}
#endif

	return *this;
}

tr::QuadInt& tr::QuadInt::operator<<=(const int count)
{
#ifdef TR_SIMD
	m_data = _mm_slli_epi32(m_data, count);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] <<= count;
	}
#endif

	return *this;
}

tr::QuadInt tr::QuadInt::operator+(const QuadInt& rhs) const
{
#ifdef TR_SIMD
	return QuadInt(_mm_add_epi32(m_data, rhs.m_data));
#else
	QuadInt result = *this;

	result += rhs;

	return result;
#endif
}

tr::QuadInt tr::QuadInt::operator*(const QuadInt& rhs) const
{
#ifdef TR_SIMD
	return QuadInt(_mm_mul_epi32(m_data, rhs.m_data));
#else
	QuadInt result = *this;

	result *= rhs;

	return result;
#endif
}

tr::QuadInt tr::QuadInt::operator|(const QuadInt& rhs) const
{
#ifdef TR_SIMD
	return QuadInt(_mm_or_si128(m_data, rhs.m_data));
#else
	return QuadInt(
		m_data[0] | rhs.m_data[0],
		m_data[1] | rhs.m_data[1],
		m_data[2] | rhs.m_data[2],
		m_data[3] | rhs.m_data[3]
	);
#endif
}

tr::QuadSizeT tr::QuadInt::toQuadSizeT() const
{
#ifdef TR_SIMD
	return QuadSizeT(_mm256_cvtepu32_epi64(m_data));
#else
	return QuadSizeT(
		size_t(m_data[0]),
		size_t(m_data[1]),
		size_t(m_data[2]),
		size_t(m_data[3])
	);
#endif
}

void tr::QuadInt::write(int32_t* const address, const QuadMask& mask) const
{
#ifdef TR_SIMD
	_mm_maskstore_epi32(address, _mm_castps_si128(mask.getData()), m_data);
#else
	uint32_t* const intPointer = reinterpret_cast<uint32_t* const>(address);
	
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		if (mask.get(i))
		{
			*(intPointer + i) = m_data[i];
		}
	}
#endif
}
