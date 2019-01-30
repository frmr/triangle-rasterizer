#include "trDefaultShader.hpp"

tr::DefaultShader::DefaultShader() :
	m_texture(nullptr),
	m_textureWrappingMode(TextureWrappingMode::Repeat),
	m_textureFiltering(false),
	m_blendMode(BlendMode::None)
{
}

void tr::DefaultShader::draw(const Vector4& position, const Vector4& worldPosition, const Vector3& normal, const Vector2& textureCoord, Color* const color, float* const depth) const
{
	const Color textureColor = m_texture->getAt(textureCoord.x, textureCoord.y, m_textureFiltering, m_textureWrappingMode);

	if (m_blendMode == BlendMode::DiscardTranslucent && textureColor.alpha < 255)
	{
		return;
	}
	else if (m_blendMode == BlendMode::WeightedAverage)
	{
		if (textureColor.alpha == 255)
		{
			*color = textureColor;
		}
		else
		{
			const Color    bufferColor = *color;
			const uint16_t totalAlpha  = textureColor.alpha + bufferColor.alpha;

			if (totalAlpha == 0)
			{
				return;
			}

			*color = Color(
				(unsigned char)((uint32_t(textureColor.blue)  * uint32_t(textureColor.alpha) + uint32_t(bufferColor.blue)  * uint32_t(bufferColor.alpha)) / totalAlpha),
				(unsigned char)((uint32_t(textureColor.green) * uint32_t(textureColor.alpha) + uint32_t(bufferColor.green) * uint32_t(bufferColor.alpha)) / totalAlpha),
				(unsigned char)((uint32_t(textureColor.red)   * uint32_t(textureColor.alpha) + uint32_t(bufferColor.red)   * uint32_t(bufferColor.alpha)) / totalAlpha),
				(unsigned char)(std::min(totalAlpha, uint16_t(255)))
			);
		}
	}
	else
	{
		*color = textureColor;
	}

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

void tr::DefaultShader::setBlendMode(const BlendMode blendMode)
{
	m_blendMode = blendMode;
}
