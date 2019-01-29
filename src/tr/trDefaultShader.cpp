#include "trDefaultShader.hpp"

tr::DefaultShader::DefaultShader() :
	m_texture(nullptr),
	m_textureWrappingMode(TextureWrappingMode::Repeat),
	m_textureFiltering(false)
{
}

void tr::DefaultShader::draw(const Vector4& position, const Vector3& normal, const Vector2& textureCoord, Color* const color, float* const depth) const
{
	*color = m_texture->getAt(textureCoord.x, textureCoord.y, m_textureFiltering, m_textureWrappingMode);
	*depth = position.z;
}

void tr::DefaultShader::setTexture(const ColorBuffer* const texture)
{
	m_texture = texture;
}

void tr::DefaultShader::setTextureWrappingMode(const TextureWrappingMode textureWrappingMode)
{
	m_textureWrappingMode = textureWrappingMode;
}

void tr::DefaultShader::setTextureFiltering(const bool textureFiltering)
{
	m_textureFiltering = textureFiltering;
}
