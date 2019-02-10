#pragma once

#include "trError.hpp"
#include "trMipBuffer.hpp"

namespace tr
{
	class ColorBuffer : public Buffer<Color>
	{
	public:
		using Buffer<Color>::Buffer;

		                       ColorBuffer(const std::string& filename);
		Error                  GenerateMipmaps();

	private:
		void                   copyTextureData(const std::vector<unsigned char>& decodedData);
		static bool            isPowerOfTwo(const size_t x);

	private:
		std::vector<MipBuffer> m_mipLevels;
	};
}