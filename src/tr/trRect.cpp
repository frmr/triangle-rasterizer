#include "trRect.hpp"
#include <algorithm>

tr::Rect::Rect(const size_t minX, const size_t minY, const size_t maxX, const size_t maxY) :
	m_minX(minX),
	m_minY(minY),
	m_maxX(maxX),
	m_maxY(maxY)
{
}

tr::Rect::Rect(const std::array<TransformedVertex, 3>& vertices) :
	m_minX(size_t(std::min({ vertices[0].projectedPosition.x, vertices[1].projectedPosition.x, vertices[2].projectedPosition.x })) & s_quadAlignmentMask),
	m_minY(size_t(std::min({ vertices[0].projectedPosition.y, vertices[1].projectedPosition.y, vertices[2].projectedPosition.y }))),
	m_maxX(size_t(std::max({ vertices[0].projectedPosition.x, vertices[1].projectedPosition.x, vertices[2].projectedPosition.x }))),
	m_maxY(size_t(std::max({ vertices[0].projectedPosition.y, vertices[1].projectedPosition.y, vertices[2].projectedPosition.y })))
{
}

size_t tr::Rect::getMinX() const
{
	return m_minX;
}

size_t tr::Rect::getMaxX() const
{
	return m_maxX;
}

size_t tr::Rect::getMinY() const
{
	return m_minY;
}

size_t tr::Rect::getMaxY() const
{
	return m_maxY;
}

tr::Rect tr::Rect::intersection(const Rect& rhs) const
{
	return Rect(
		std::max(rhs.m_minX, m_minX),
		std::max(rhs.m_minY, m_minY),
		std::min(rhs.m_maxX, m_maxX),
		std::min(rhs.m_maxY, m_maxY)
	);
}

bool tr::Rect::isValid() const
{
	return m_minX < m_maxX && m_minY < m_maxY;
}
