#pragma once

#include "trRect.hpp"

namespace tr
{
	class Tile
	{
	public:
		            Tile(const Rect boundingBox);

		const Rect& getBounds() const;

	private:
		const Rect  m_bounds;
	};
}