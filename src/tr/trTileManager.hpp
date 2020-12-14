#pragma once

#include <vector>
#include "trTile.hpp"

namespace tr
{
	class TileManager
	{
	public:
		                  TileManager(const size_t viewportWidth, const size_t viewportHeight, const size_t tileWidth, const size_t tileHeight);

		void              setAttributes(const size_t viewportWidth, const size_t viewportHeight, const size_t tileWidth, const size_t tileHeight);

	private:
		std::vector<Tile> m_tiles;
	};
}