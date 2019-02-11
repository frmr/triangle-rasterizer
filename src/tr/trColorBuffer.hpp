#pragma once

#include "trError.hpp"
#include "trMipBuffer.hpp"
#include <cstdint>

namespace tr
{
	class ColorBuffer : public Buffer<Color>
	{
	public:
		using Buffer<Color>::Buffer;

		                       ColorBuffer(const std::string& filename);
		Error                  GenerateMipmaps();

	private:
		void                   copyTextureData(const std::vector<uint8_t>& decodedData);
		static bool            isPowerOfTwo(const size_t x);

	private:
		std::vector<MipBuffer> m_mipLevels;
	};
}