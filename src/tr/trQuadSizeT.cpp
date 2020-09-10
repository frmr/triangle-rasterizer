#include "trQuadSizeT.hpp"

tr::QuadSizeT::QuadSizeT(const size_t a) :
	m_data{ a, a, a, a }
{
}

tr::QuadSizeT::QuadSizeT(const size_t a, const size_t b, const size_t c, const size_t d) :
	m_data{ a, b, c, d }
{
}

const std::array<size_t, 4>& tr::QuadSizeT::getData() const
{
	return m_data;
}

tr::QuadSizeT& tr::QuadSizeT::operator+=(const QuadSizeT& rhs)
{
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] += rhs.m_data[i];
	}

	return *this;
}

tr::QuadSizeT& tr::QuadSizeT::operator*=(const QuadSizeT& rhs)
{
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] *= rhs.m_data[i];
	}

	return *this;
}

tr::QuadSizeT tr::QuadSizeT::operator+(const QuadSizeT& rhs) const
{
	QuadSizeT result = *this;

	result += rhs;

	return result;
}

tr::QuadSizeT tr::QuadSizeT::operator*(const QuadSizeT& rhs) const
{
	QuadSizeT result = *this;

	result *= rhs;

	return result;
}

size_t tr::QuadSizeT::get(const size_t index) const
{
	return m_data[index];
}
