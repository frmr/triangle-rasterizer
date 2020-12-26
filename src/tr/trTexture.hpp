#pragma once

#include "trColorBuffer.hpp"
#include "trQuadColor.hpp"

namespace tr
{
	class Texture
	{
	public:
		                         Texture(const size_t width, const size_t height);
		                         Texture(const size_t width, const size_t height, const std::vector<uint8_t>& rgbaData);

		bool                     isInitialized() const;
		void                     generateMipmaps();
		size_t                   getWidth() const;
		size_t                   getHeight() const;
		ColorBuffer&             getMipLevel(const size_t mipLevel);
		const ColorBuffer&       getConstMipLevel(const size_t mipLevel) const;
		size_t                   getNumMipLevels() const;
		QuadColor                getAt(const QuadFloat& u, const QuadFloat& v, const bool filter, const TextureWrappingMode textureWrappingMode, const QuadMask& mask) const;
		//Color                    getAt(const float u, const float v, const bool filter, const TextureWrappingMode textureWrappingMode, const float du, const float dv, const bool interpolateMipmapLevels) const;
		QuadColor                getAt(const QuadFloat& u, const QuadFloat& v, const QuadMask& mask) const;

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