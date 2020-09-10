#include "trQuadInt.hpp"

tr::QuadInt::QuadInt(const int32_t a) :
	m_data{ a, a, a, a }
{
}

tr::QuadInt::QuadInt(const int32_t a, const int32_t b, const int32_t c, const int32_t d) :
	m_data{ a, b, c, d }
{
}

const std::array<int32_t, 4>& tr::QuadInt::getData() const
{
	return m_data;
}

tr::QuadInt& tr::QuadInt::operator+=(const QuadInt& rhs)
{
	for (int32_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] += rhs.m_data[i];
	}

	return *this;
}

tr::QuadInt& tr::QuadInt::operator*=(const QuadInt& rhs)
{
	for (int32_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] *= rhs.m_data[i];
	}

	return *this;
}

tr::QuadInt tr::QuadInt::operator+(const QuadInt& rhs) const
{
	QuadInt result = *this;

	result += rhs;

	return result;
}

tr::QuadInt tr::QuadInt::operator*(const QuadInt& rhs) const
{
	QuadInt result = *this;

	result += rhs;

	return result;
}