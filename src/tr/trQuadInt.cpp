#include "trQuadInt.hpp"
#include "trQuadFloat.hpp"

// TODO: Find a better place for this
const __m128i allZeroes = _mm_setzero_si128();

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

tr::QuadInt& tr::QuadInt::operator-=(const QuadInt& rhs)
{
#ifdef TR_SIMD
	m_data = _mm_sub_epi32(m_data, rhs.m_data);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] -= rhs.m_data[i];
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

tr::QuadInt& tr::QuadInt::operator>>=(const int count)
{
#ifdef TR_SIMD
	m_data = _mm_srli_epi32(m_data, count);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] >>= count;
	}
#endif

	return *this;
}

tr::QuadInt tr::QuadInt::operator<<(const int count) const
{
#ifdef TR_SIMD
	return QuadInt(_mm_slli_epi32(m_data, count));
#else
	QuadInt result = *this;

	result <<= count;

	return result;
#endif
}

tr::QuadInt tr::QuadInt::operator>>(const int count) const
{
#ifdef TR_SIMD
	return QuadInt(_mm_srli_epi32(m_data, count));
#else
	QuadInt result = *this;

	result >>= count;

	return result;
#endif
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

tr::QuadInt tr::QuadInt::operator-(const QuadInt& rhs) const
{
#ifdef TR_SIMD
	return QuadInt(_mm_sub_epi32(m_data, rhs.m_data));
#else
	QuadInt result = *this;

	result -= rhs;

	return result;
#endif
}

tr::QuadInt tr::QuadInt::operator*(const QuadInt& rhs) const
{
#ifdef TR_SIMD
	return QuadInt(_mm_mullo_epi32(m_data, rhs.m_data));
#else
	QuadInt result = *this;

	result *= rhs;

	return result;
#endif
}

tr::QuadInt tr::QuadInt::operator&(const QuadInt& rhs) const
{
#ifdef TR_SIMD
	return QuadInt(_mm_and_si128(m_data, rhs.m_data));
#else
	return QuadInt(
		m_data[0] & rhs.m_data[0],
		m_data[1] & rhs.m_data[1],
		m_data[2] & rhs.m_data[2],
		m_data[3] & rhs.m_data[3]
	);
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

tr::QuadMask tr::QuadInt::equal(const QuadInt& rhs) const
{
#ifdef TR_SIMD
	return QuadMask(_mm_castsi128_ps(_mm_cmpeq_epi32(m_data, rhs.m_data)));
#else
	return QuadMask(
		m_data[0] == rhs.m_data[0],
		m_data[1] == rhs.m_data[1],
		m_data[2] == rhs.m_data[2],
		m_data[3] == rhs.m_data[3]
	);
#endif
}

tr::QuadInt tr::QuadInt::maskedCopy(const QuadInt& rhs, const QuadMask& mask) const
{
#ifdef TR_SIMD
	return QuadInt(_mm_blendv_epi8(m_data, rhs.m_data, _mm_castps_si128(mask.getData())));
#else
	return QuadInt(
		mask.get(0) ? rhs.m_data[0] : m_data[0],
		mask.get(1) ? rhs.m_data[1] : m_data[1],
		mask.get(2) ? rhs.m_data[2] : m_data[2],
		mask.get(3) ? rhs.m_data[3] : m_data[3]
	);
#endif
}

tr::QuadFloat tr::QuadInt::convertToQuadFloat() const
{
#ifdef TR_SIMD
	return QuadFloat(_mm_cvtepi32_ps(m_data));
#else
	return QuadFloat(
		float(m_data[0]),
		float(m_data[1]),
		float(m_data[2]),
		float(m_data[3])
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

tr::QuadInt tr::QuadInt::gatherIntsAtOffsets(const int32_t* const baseAddress, const QuadMask& mask) const
{
#ifdef TR_SIMD
	return QuadInt(_mm_mask_i32gather_epi32(allZeroes, baseAddress, m_data, _mm_castps_si128(mask.getData()), 4));
#else
	return QuadInt(
		mask.get(0) ? *(baseAddress + m_data[0]) : 0,
		mask.get(1) ? *(baseAddress + m_data[1]) : 0,
		mask.get(2) ? *(baseAddress + m_data[2]) : 0,
		mask.get(3) ? *(baseAddress + m_data[3]) : 0
	);
#endif
}
