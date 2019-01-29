#pragma once

#include "trColorBuffer.hpp"

namespace tr
{
	class DefaultShader
	{
	public:
		                    DefaultShader();

		void                draw(const Vector4& position, const Vector3& normal, const Vector2& textureCoord, Color* const color, float* const depth) const;

		void                setTexture(const ColorBuffer* const texture);
		void                setTextureWrappingMode(const TextureWrappingMode textureWrappingMode);
		void                setTextureFiltering(const bool textureFiltering);

	private:
		const ColorBuffer*  m_texture;
		TextureWrappingMode	m_textureWrappingMode;
		bool                m_textureFiltering;
	};
}