#include "trRect.hpp"
#include <algorithm>
#include <cmath>

tr::Rect::Rect(const int32_t minX, const int32_t minY, const int32_t maxX, const int32_t maxY) :
	m_minX(minX),
	m_minY(minY),
	m_maxX(maxX),
	m_maxY(maxY)
{
}

tr::Rect::Rect(const std::array<TransformedVertex, 3>& vertices) :
	m_minX(int32_t(std::min({ vertices[0].projectedPosition.x, vertices[1].projectedPosition.x, vertices[2].projectedPosition.x }))),
	m_minY(int32_t(std::min({ vertices[0].projectedPosition.y, vertices[1].projectedPosition.y, vertices[2].projectedPosition.y }))),
	m_maxX(int32_t(std::max({ vertices[0].projectedPosition.x, vertices[1].projectedPosition.x, vertices[2].projectedPosition.x }))),
	m_maxY(int32_t(std::max({ vertices[0].projectedPosition.y, vertices[1].projectedPosition.y, vertices[2].projectedPosition.y })))
{
}

int32_t tr::Rect::getMinX() const
{
	return m_minX;
}

int32_t tr::Rect::getMaxX() const
{
	return m_maxX;
}

int32_t tr::Rect::getMinY() const
{
	return m_minY;
}

int32_t tr::Rect::getMaxY() const
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
	return m_minX >= 0 && m_minY >= 0 && m_minX <= m_maxX && m_minY <= m_maxY;
}
