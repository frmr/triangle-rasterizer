#include "trDepthShader.hpp"

void tr::DepthShader::draw(const Vector4& position, const Vector4& worldPosition, const Vector3& normal, const Vector2& textureCoord, Color* const color, double* const depth) const
{
	const unsigned char value = 255 - (unsigned char)((position.z + 1.0f) * 127.0f);

	*color = Color(value, value, value, 255);
	*depth = position.z;
}
