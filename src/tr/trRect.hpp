#pragma once

#include "trTransformedVertex.hpp"
#include <array>
#include <cstdint>

namespace tr
{
	class Rect
	{
	public:
		        Rect(const int32_t minX, const int32_t minY, const int32_t maxX, const int32_t maxY);
		        Rect(const std::array<TransformedVertex, 3>& vertices);

		int32_t getMinX() const;
		int32_t getMaxX() const;
		int32_t getMinY() const;
		int32_t getMaxY() const;

		Rect    intersection(const Rect& rhs) const;
		bool    isValid() const;

	private:
		int32_t m_minX;
		int32_t m_minY;
		int32_t m_maxX;
		int32_t m_maxY;
	};
}