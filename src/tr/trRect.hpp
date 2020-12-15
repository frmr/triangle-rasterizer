#pragma once

#include "trTransformedVertex.hpp"
#include <array>

namespace tr
{
	class Rect
	{
	public:
		                        Rect(const size_t minX, const size_t minY, const size_t maxX, const size_t maxY);
		                        Rect(const std::array<TransformedVertex, 3>& vertices);

		size_t                  getMinX() const;
		size_t                  getMaxX() const;
		size_t                  getMinY() const;
		size_t                  getMaxY() const;

		Rect                    intersection(const Rect& rhs) const;
		bool                    isValid() const;

	private:
		static constexpr size_t s_quadAlignmentMask = std::numeric_limits<size_t>::max() ^ 0x03;

		size_t                  m_minX;
		size_t                  m_minY;
		size_t                  m_maxX;
		size_t                  m_maxY;
	};
}