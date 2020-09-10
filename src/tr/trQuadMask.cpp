#include "trQuadMask.hpp"

tr::QuadMask::QuadMask(const bool a) :
	m_data{ a, a, a, a }
{
}

tr::QuadMask::QuadMask(const bool a, const bool b, const bool c, const bool d) :
	m_data{ a, b, c, d }
{
}

tr::QuadMask& tr::QuadMask::operator&=(const QuadMask& rhs)
{
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		m_data[i] &= rhs.m_data[i];
	}

	return *this;
}

tr::QuadMask tr::QuadMask::operator|(const QuadMask& rhs) const
{
	return QuadMask(
		m_data[0] || rhs.m_data[0],
		m_data[1] || rhs.m_data[1],
		m_data[2] || rhs.m_data[2],
		m_data[3] || rhs.m_data[3]
	);
}

bool tr::QuadMask::get(const size_t index) const
{
	return m_data[index];
}

bool tr::QuadMask::moveMask() const
{
	return m_data[0] || m_data[1] || m_data[2] || m_data[3];
}
