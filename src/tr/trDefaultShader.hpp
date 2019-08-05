#pragma once

#include "trTexture.hpp"
#include "trBlendMode.hpp"

namespace tr
{
	class DefaultShader
	{
	public:
		                    DefaultShader();

		void                draw(const Vector4& screenPosition, const Vector3& worldPosition, const Vector3& normal, const Vector2& textureCoord, Color& color, float& depth) const;

		void                setTexture(const Texture* const texture);
		void                setTextureWrappingMode(const TextureWrappingMode textureWrappingMode);
		void                setTextureFiltering(const bool textureFiltering);
		void                setBlendMode(const BlendMode blendMode);

	private:
		const Texture*      m_texture;
		TextureWrappingMode	m_textureWrappingMode;
		bool                m_textureFiltering;
		BlendMode           m_blendMode;
	};
}