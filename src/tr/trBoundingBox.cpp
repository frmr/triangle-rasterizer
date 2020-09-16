#include "trBoundingBox.hpp"
#include <algorithm>

tr::BoundingBox::BoundingBox(const std::array<TransformedVertex, 3>& vertices) :
	m_minX(size_t(std::min({ vertices[0].projectedPosition.x, vertices[1].projectedPosition.x, vertices[2].projectedPosition.x })) & s_quadAlignmentMask),
	m_minY(size_t(std::min({ vertices[0].projectedPosition.y, vertices[1].projectedPosition.y, vertices[2].projectedPosition.y }))),
	m_maxX(size_t(std::max({ vertices[0].projectedPosition.x, vertices[1].projectedPosition.x, vertices[2].projectedPosition.x }))),
	m_maxY(size_t(std::max({ vertices[0].projectedPosition.y, vertices[1].projectedPosition.y, vertices[2].projectedPosition.y })))
{
}

size_t tr::BoundingBox::getMinX() const
{
	return m_minX;
}

size_t tr::BoundingBox::getMaxX() const
{
	return m_maxX;
}

size_t tr::BoundingBox::getMinY() const
{
	return m_minY;
}

size_t tr::BoundingBox::getMaxY() const
{
	return m_maxY;
}
