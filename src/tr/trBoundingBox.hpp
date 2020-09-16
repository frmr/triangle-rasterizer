#pragma once

#include "trTransformedVertex.hpp"
#include <array>

namespace tr
{
	class BoundingBox
	{
	public:
		                        BoundingBox(const std::array<TransformedVertex, 3>& vertices);

		size_t                  getMinX() const;
		size_t                  getMaxX() const;
		size_t                  getMinY() const;
		size_t                  getMaxY() const;

	private:
		static constexpr size_t s_quadAlignmentMask = std::numeric_limits<size_t>::max() ^ 0x03;

		size_t                  m_minX;
		size_t                  m_maxX;
		size_t                  m_minY;
		size_t                  m_maxY;
	};
}