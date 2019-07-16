#include "trDepthShader.hpp"

void tr::DepthShader::draw(const Vector4& position, const Vector4& worldPosition, const Vector3& normal, const Vector2& textureCoord, tr::Color& color, float& depth) const
{
	const uint8_t value = 255 - uint8_t((position.z + 1.0f) * 127.0f);

	color = Color(value, value, value, 255);
	depth = position.z;
}
