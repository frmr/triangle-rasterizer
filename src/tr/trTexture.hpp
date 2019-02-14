#pragma once

#include "trColorBuffer.hpp"
#include "trError.hpp"

namespace tr
{
	class Texture
	{
	public:
		                         Texture(const size_t width, const size_t height);
		                         Texture(const std::string& filename);

		Error                    generateMipmaps();
		size_t                   getWidth() const;
		size_t                   getHeight() const;
		ColorBuffer&             getMipLevel(const size_t mipLevel);
		const ColorBuffer&       getConstMipLevel(const size_t mipLevel) const;
		size_t                   getNumMipLevels() const;
		Color                    getAt(float u, float v, const bool filter, const TextureWrappingMode textureWrappingMode) const;
		Color                    getAt(float u, float v, const bool filter, const TextureWrappingMode textureWrappingMode, const float du, const float dv, const bool interpolateMipmapLevels) const;

	private:
		void                     init(const size_t width, const size_t height);
		void                     copyImageDataToBaseLevel(const std::vector<uint8_t>& decodedData);
		static bool              isPowerOfTwo(const size_t x);
		static float             fastLog2(const float x);

	private:
		std::vector<ColorBuffer> m_mipLevels;
		size_t                   m_maxMipLevelIndex;
		ColorBuffer*             m_baseLevel;
	};
}