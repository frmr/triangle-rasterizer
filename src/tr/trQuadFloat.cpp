#include "trQuadFloat.hpp"
#include "trQuadInt.hpp"
#include <cmath>

tr::QuadFloat::QuadFloat(const float a) :
#ifdef TR_SIMD
	m_data(_mm_set1_ps(a))
#else
	m_data{ a, a, a, a }
#endif
{
}

tr::QuadFloat::QuadFloat(const float a, const float b, const float c, const float d) :
#ifdef TR_SIMD
	m_data(_mm_setr_ps(a, b, c, d))
#else
	m_data{ a, b, c, d }
#endif
{
}

tr::QuadFloat::QuadFloat(const float* const pointer) :
#ifdef TR_SIMD
	m_data(_mm_load_ps(pointer))
#else
	m_data{ *pointer, *(pointer + 1), *(pointer + 2), *(pointer + 3) }
#endif
{
}

tr::QuadFloat::QuadFloat(const float* const pointer, const QuadMask& mask) :
#ifdef TR_SIMD
	m_data(_mm_maskload_ps(pointer, _mm_castps_si128(mask.getData())))
#else
	m_data{ 
		mask.get(0) ? (*pointer + 0) : 0.0f,
		mask.get(1) ? (*pointer + 1) : 0.0f,
		mask.get(2) ? (*pointer + 2) : 0.0f,
		mask.get(3) ? (*pointer + 3) : 0.0f
	}
#endif
{
}

#ifdef TR_SIMD
tr::QuadFloat::QuadFloat(const __m128 data) :
	m_data(data)
{
}
#endif

tr::QuadFloat& tr::QuadFloat::operator+=(const QuadFloat& rhs)
{
#ifdef TR_SIMD
	m_data = _mm_add_ps(m_data, rhs.m_data);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] += rhs.m_data[i];
	}
#endif

	return *this;
}

tr::QuadFloat& tr::QuadFloat::operator-=(const QuadFloat& rhs)
{
#ifdef TR_SIMD
	m_data = _mm_sub_ps(m_data, rhs.m_data);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] -= rhs.m_data[i];
	}
#endif

	return *this;
}

tr::QuadFloat& tr::QuadFloat::operator*=(const QuadFloat& rhs)
{
#ifdef TR_SIMD
	m_data = _mm_mul_ps(m_data, rhs.m_data);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] *= rhs.m_data[i];
	}
#endif

	return *this;
}

tr::QuadFloat& tr::QuadFloat::operator/=(const QuadFloat& rhs)
{
#ifdef TR_SIMD
	m_data = _mm_div_ps(m_data, rhs.m_data);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] /= rhs.m_data[i];
	}
#endif

	return *this;
}

tr::QuadFloat tr::QuadFloat::operator+(const QuadFloat& rhs) const
{
#ifdef TR_SIMD
	return QuadFloat(_mm_add_ps(m_data, rhs.m_data));
#else
	QuadFloat result = *this;

	result += rhs;

	return result;
#endif
}

tr::QuadFloat tr::QuadFloat::operator-(const QuadFloat& rhs) const
{
#ifdef TR_SIMD
	return QuadFloat(_mm_sub_ps(m_data, rhs.m_data));
#else
	QuadFloat result = *this;

	result -= rhs;

	return result;
#endif
}

tr::QuadFloat tr::QuadFloat::operator*(const QuadFloat& rhs) const
{
#ifdef TR_SIMD
	return QuadFloat(_mm_mul_ps(m_data, rhs.m_data));
#else
	QuadFloat result = *this;

	result *= rhs;

	return result;
#endif
}

tr::QuadFloat tr::QuadFloat::operator/(const QuadFloat& rhs) const
{
#ifdef TR_SIMD
	return QuadFloat(_mm_div_ps(m_data, rhs.m_data));
#else
	QuadFloat result = *this;

	result /= rhs;

	return result;
#endif
}

tr::QuadFloat tr::QuadFloat::operator&(const QuadFloat& rhs) const
{
#ifdef TR_SIMD
	return QuadFloat(_mm_and_ps(m_data, rhs.m_data));
#else
	const uint32_t int0 = *reinterpret_cast<const uint32_t*>(&m_data[0]) & *reinterpret_cast<const uint32_t*>(&rhs.m_data[0]);
	const uint32_t int1 = *reinterpret_cast<const uint32_t*>(&m_data[1]) & *reinterpret_cast<const uint32_t*>(&rhs.m_data[1]);
	const uint32_t int2 = *reinterpret_cast<const uint32_t*>(&m_data[2]) & *reinterpret_cast<const uint32_t*>(&rhs.m_data[2]);
	const uint32_t int3 = *reinterpret_cast<const uint32_t*>(&m_data[3]) & *reinterpret_cast<const uint32_t*>(&rhs.m_data[3]);

	return QuadFloat(
		*reinterpret_cast<const float*>(&int0),
		*reinterpret_cast<const float*>(&int1),
		*reinterpret_cast<const float*>(&int2),
		*reinterpret_cast<const float*>(&int3)
	);
#endif
}

tr::QuadFloat tr::QuadFloat::operator|(const QuadFloat& rhs) const
{
#ifdef TR_SIMD
	return QuadFloat(_mm_or_ps(m_data, rhs.m_data));
#else
	const uint32_t int0 = *reinterpret_cast<const uint32_t*>(&m_data[0]) | *reinterpret_cast<const uint32_t*>(&rhs.m_data[0]);
	const uint32_t int1 = *reinterpret_cast<const uint32_t*>(&m_data[1]) | *reinterpret_cast<const uint32_t*>(&rhs.m_data[1]);
	const uint32_t int2 = *reinterpret_cast<const uint32_t*>(&m_data[2]) | *reinterpret_cast<const uint32_t*>(&rhs.m_data[2]);
	const uint32_t int3 = *reinterpret_cast<const uint32_t*>(&m_data[3]) | *reinterpret_cast<const uint32_t*>(&rhs.m_data[3]);

	return QuadFloat(
		*reinterpret_cast<const float*>(&int0),
		*reinterpret_cast<const float*>(&int1),
		*reinterpret_cast<const float*>(&int2),
		*reinterpret_cast<const float*>(&int3)
	);
#endif
}

tr::QuadMask tr::QuadFloat::greaterThan(const QuadFloat& rhs) const
{
#ifdef TR_SIMD
	return QuadMask(_mm_cmpgt_ps(m_data, rhs.m_data));
#else
	return QuadMask(
		m_data[0] > rhs.m_data[0],
		m_data[1] > rhs.m_data[1],
		m_data[2] > rhs.m_data[2],
		m_data[3] > rhs.m_data[3]
	);
#endif
}

tr::QuadMask tr::QuadFloat::lessThan(const QuadFloat& rhs) const
{
#ifdef TR_SIMD
	return QuadMask(_mm_cmplt_ps(m_data, rhs.m_data));
#else
	return QuadMask(
		m_data[0] < rhs.m_data[0],
		m_data[1] < rhs.m_data[1],
		m_data[2] < rhs.m_data[2],
		m_data[3] < rhs.m_data[3]
	);
#endif
}

tr::QuadMask tr::QuadFloat::castToMask() const
{
#ifdef TR_SIMD
	return QuadMask(m_data);
#else
	const uint32_t int0 = *reinterpret_cast<const uint32_t*>(&m_data[0]);
	const uint32_t int1 = *reinterpret_cast<const uint32_t*>(&m_data[1]);
	const uint32_t int2 = *reinterpret_cast<const uint32_t*>(&m_data[2]);
	const uint32_t int3 = *reinterpret_cast<const uint32_t*>(&m_data[3]);

	return QuadMask(
		(int0 & 0x80000000) > 0,
		(int1 & 0x80000000) > 0,
		(int2 & 0x80000000) > 0,
		(int3 & 0x80000000) > 0
	);
#endif
}

tr::QuadFloat tr::QuadFloat::abs() const
{
#ifdef TR_SIMD
	__m128i intData = _mm_castps_si128(m_data);

	intData = _mm_slli_epi32(intData, 1);
	intData = _mm_srli_epi32(intData, 1);
	
	return QuadFloat(_mm_castsi128_ps(intData));
#else
	return QuadFloat(
		std::abs(m_data[0]),
		std::abs(m_data[1]),
		std::abs(m_data[2]),
		std::abs(m_data[3])
	);
#endif
}

tr::QuadFloat tr::QuadFloat::floor() const
{
#ifdef TR_SIMD
	return QuadFloat(_mm_floor_ps(m_data));
#else
	QuadFloat result = *this;

	for (float& element : result.m_data)
	{
		element = std::floor(element);
	}

	return result;
#endif
}

tr::QuadFloat tr::QuadFloat::round() const
{
#ifdef TR_SIMD
	return QuadFloat(_mm_round_ps(m_data, _MM_FROUND_TO_NEAREST_INT));
#else
	QuadFloat result = *this;

	for (float& element : result.m_data)
	{
		element = std::round(element);
	}

	return result;
#endif
}

tr::QuadInt tr::QuadFloat::convertToQuadInt() const
{
#if TR_SIMD
	return QuadInt(_mm_cvttps_epi32(m_data));
#else
	return QuadInt(
		int32_t(m_data[0]),
		int32_t(m_data[1]),
		int32_t(m_data[2]),
		int32_t(m_data[3])
	);
#endif
}

void tr::QuadFloat::write(float* const pointer, const QuadMask& mask) const
{
#ifdef TR_SIMD
	_mm_maskstore_ps(pointer, _mm_castps_si128(mask.getData()), m_data);
#else
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		*(pointer + i) = m_data[i];
	}
#endif
}