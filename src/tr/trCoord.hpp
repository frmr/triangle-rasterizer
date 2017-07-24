#pragma once

namespace tr
{
	struct Coord
	{
	public:
		Coord(const int x, const int y, const float depth) :
			x(x), y(y), depth(depth)
		{
		}

		bool IsValid(const int screenWidth, const int screenHeight) const
		{
			return x >= 0 && x < screenWidth && y >= 0 && y < screenHeight && depth >= -1.0 && depth <= 1.0;
		}

	public:
		int   x;
		int   y;
		float depth;
	};
}