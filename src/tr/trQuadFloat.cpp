#include "trQuadFloat.hpp"
#include <cmath>

tr::QuadFloat::QuadFloat(const float a) :
	m_data{ a, a, a, a }
{
}

tr::QuadFloat::QuadFloat(const float a, const float b, const float c, const float d) :
	m_data{ a, b, c, d }
{
}

tr::QuadFloat::QuadFloat(const float* const pointer) :
	m_data{ *pointer, *(pointer + 1), *(pointer + 2), *(pointer + 3) }
{
}

tr::QuadFloat& tr::QuadFloat::operator+=(const QuadFloat& rhs)
{
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] += rhs.m_data[i];
	}

	return *this;
}

tr::QuadFloat& tr::QuadFloat::operator-=(const QuadFloat& rhs)
{
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] -= rhs.m_data[i];
	}

	return *this;
}

tr::QuadFloat& tr::QuadFloat::operator*=(const QuadFloat& rhs)
{
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] *= rhs.m_data[i];
	}

	return *this;
}

tr::QuadFloat& tr::QuadFloat::operator/=(const QuadFloat& rhs)
{
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] /= rhs.m_data[i];
	}

	return *this;
}

tr::QuadFloat tr::QuadFloat::operator+(const QuadFloat& rhs) const
{
	QuadFloat result = *this;

	result += rhs;

	return result;
}

tr::QuadFloat tr::QuadFloat::operator-(const QuadFloat& rhs) const
{
	QuadFloat result = *this;

	result -= rhs;

	return result;
}

tr::QuadFloat tr::QuadFloat::operator*(const QuadFloat& rhs) const
{
	QuadFloat result = *this;

	result *= rhs;

	return result;
}

tr::QuadFloat tr::QuadFloat::operator/(const QuadFloat& rhs) const
{
	QuadFloat result = *this;

	result /= rhs;

	return result;
}

tr::QuadMask tr::QuadFloat::greaterThan(const QuadFloat& rhs) const
{
	return QuadMask(
		m_data[0] > rhs.m_data[0],
		m_data[1] > rhs.m_data[1],
		m_data[2] > rhs.m_data[2],
		m_data[3] > rhs.m_data[3]
	);
}

tr::QuadMask tr::QuadFloat::lessThan(const QuadFloat& rhs) const
{
	return QuadMask(
		m_data[0] < rhs.m_data[0],
		m_data[1] < rhs.m_data[1],
		m_data[2] < rhs.m_data[2],
		m_data[3] < rhs.m_data[3]
	);
}

const tr::QuadFloat& tr::QuadFloat::abs()
{
	for (float& element : m_data)
	{
		element = std::abs(element);
	}

	return *this;
}

tr::QuadFloat tr::QuadFloat::floor() const
{
	QuadFloat result = *this;

	for (float& element : result.m_data)
	{
		element = std::floor(element);
	}

	return result;
}

tr::QuadSizeT tr::QuadFloat::toQuadSizeT() const
{
	return QuadSizeT(
		size_t(m_data[0]),
		size_t(m_data[1]),
		size_t(m_data[2]),
		size_t(m_data[3])
	);
}

float tr::QuadFloat::get(const size_t index) const
{
	return m_data[index];
}

void tr::QuadFloat::write(float* const pointer) const
{
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		*(pointer + i) = m_data[i];
	}
}
