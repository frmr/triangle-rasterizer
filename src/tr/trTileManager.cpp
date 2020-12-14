#include "trTileManager.hpp"

tr::TileManager::TileManager(const size_t viewportWidth, const size_t viewportHeight, const size_t tileWidth, const size_t tileHeight)
{
	setAttributes(viewportWidth, viewportHeight, tileWidth, tileHeight);
}

void tr::TileManager::setAttributes(const size_t viewportWidth, const size_t viewportHeight, const size_t tileWidth, const size_t tileHeight)
{
	m_tiles.clear();

	for (size_t y = 0; y < viewportHeight; y += tileHeight)
	{
		size_t tileMaxY = y + tileHeight - 1;

		if (tileMaxY >= viewportHeight)
		{
			tileMaxY = viewportHeight - 1;
		}

		for (size_t x = 0; x < viewportWidth; x += tileWidth)
		{
			size_t tileMaxX = x + tileWidth - 1;

			if (tileMaxX >= viewportWidth)
			{
				tileMaxX = viewportWidth - 1;
			}

			m_tiles.emplace_back(Rect(x, y, tileMaxX, tileMaxY));
		}
	}
}
