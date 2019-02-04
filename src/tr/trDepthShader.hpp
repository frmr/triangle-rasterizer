#pragma once

#include "../matrix/Vectors.h"
#include "trColor.hpp"

namespace tr
{
	class DepthShader
	{
	public:
		void draw(const Vector4& position, const Vector4& worldPosition, const Vector3& normal, const Vector2& textureCoord, Color* const color, double* const depth) const;
	};
}