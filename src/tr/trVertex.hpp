#pragma once

#include "../matrix/Vectors.h"

namespace tr
{
	struct Vertex
	{
		Vector4 position;
		Vector3 normal;
		Vector2 textureCoord;
	};
}