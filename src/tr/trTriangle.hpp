#pragma once

#include "trVertex.hpp"

#include <array>

namespace tr
{
	struct Triangle
	{
		std::array<Vertex, 3> vertices;
	};
}