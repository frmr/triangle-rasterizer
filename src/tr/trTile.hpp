#pragma once

#include "trRect.hpp"

namespace tr
{
	class Tile
	{
	public:
		           Tile(const Rect boundingBox);

	private:
		const Rect m_bounds;
	};
}