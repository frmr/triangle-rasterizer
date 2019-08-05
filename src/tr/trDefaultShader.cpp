#include "trDefaultShader.hpp"

tr::DefaultShader::DefaultShader() :
	m_texture(nullptr),
	m_textureWrappingMode(TextureWrappingMode::Repeat),
	m_textureFiltering(false),
	m_blendMode(BlendMode::None)
{
}

void tr::DefaultShader::draw(const Vector4& screenPosition, const Vector3& worldPosition, const Vector3& normal, const Vector2& textureCoord, Color& color, float& depth) const
{
	//const Color textureColor = m_texture->getAt(textureCoord.x, textureCoord.y, m_textureFiltering, m_textureWrappingMode, 0.105f, 0.105f, true);
	const Color textureColor = m_texture->getAt(textureCoord.x, textureCoord.y, m_textureFiltering, m_textureWrappingMode);

	if (m_blendMode == BlendMode::DiscardTranslucent && textureColor.a < 255)
	{
		return;
	}
	else if (m_blendMode == BlendMode::WeightedAverage)
	{
		if (textureColor.a == 255)
		{
			color = textureColor;
		}
		else if (textureColor.a == 0)
		{
			return;
		}
		else
		{
			const Color    bufferColor = color;
			const uint16_t totalAlpha  = textureColor.a + bufferColor.a;

			if (totalAlpha == 0)
			{
				return;
			}

			color = Color(
				uint8_t(((uint32_t(textureColor.b) * uint32_t(textureColor.a) + uint32_t(bufferColor.b) * uint32_t(bufferColor.a)) / totalAlpha)),
				uint8_t(((uint32_t(textureColor.g) * uint32_t(textureColor.a) + uint32_t(bufferColor.g) * uint32_t(bufferColor.a)) / totalAlpha)),
				uint8_t(((uint32_t(textureColor.r) * uint32_t(textureColor.a) + uint32_t(bufferColor.r) * uint32_t(bufferColor.a)) / totalAlpha)),
				uint8_t((std::min(totalAlpha, uint16_t(255))))
			);
		}
	}
	else
	{
		color = textureColor;
	}

	depth = screenPosition.z;
}

void tr::DefaultShader::setTexture(const Texture* const texture)
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

void tr::DefaultShader::setBlendMode(const BlendMode blendMode)
{
	m_blendMode = blendMode;
}
