#include "trTile.hpp"

tr::Tile::Tile(const Rect bounds) :
	m_bounds(bounds)
{
}

const tr::Rect& tr::Tile::getBounds() const
{
	return m_bounds;
}
