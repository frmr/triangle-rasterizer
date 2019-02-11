#include "trColorBuffer.hpp"

#include "../lodepng/lodepng.h"

tr::ColorBuffer::ColorBuffer(const std::string& filename) :
	Buffer<Color>()
{
	std::vector<uint8_t> encodedData;

	if (!lodepng::load_file(encodedData, filename))
	{
		uint32_t             width;
		uint32_t             height;
		lodepng::State       state;
		std::vector<uint8_t> decodedData;

		state.info_raw.colortype = LodePNGColorType::LCT_RGBA;

		if (!lodepng::decode(decodedData, width, height, state, encodedData))
		{
			init(width, height);
			copyTextureData(decodedData);
		}
	}
}

tr::Error tr::ColorBuffer::GenerateMipmaps()
{
	if (!isPowerOfTwo(m_width) || !isPowerOfTwo(m_height))
	{
		return Error::InvalidBufferSize;
	}

	Buffer<Color>& source = *this;

	// TODO: Preallocate m_mipLevels

	do
	{
		m_mipLevels.emplace_back(source.getHeight() / 2, source.getHeight() / 2);

		Buffer<Color>& dest = m_mipLevels.back();

		for (size_t sourceY = 0, destY = 0; sourceY < source.getHeight(); sourceY += 2, ++destY)
		{
			for (size_t sourceX = 0, destX = 0; sourceX < source.getWidth(); sourceX += 2, ++destX)
			{
				Color& tl = source.at(sourceX,     sourceY    );
				Color& tr = source.at(sourceX + 1, sourceY    );
				Color& bl = source.at(sourceX,     sourceY + 1);
				Color& br = source.at(sourceX + 1, sourceY + 1);

				dest.at(destX, destY) = Color(uint8_t((uint16_t(tl.b) + uint16_t(tr.b) + uint16_t(bl.b) + uint16_t(br.b)) / 4),
				                              uint8_t((uint16_t(tl.g) + uint16_t(tr.g) + uint16_t(bl.g) + uint16_t(br.g)) / 4),
				                              uint8_t((uint16_t(tl.r) + uint16_t(tr.r) + uint16_t(bl.r) + uint16_t(br.r)) / 4),
				                              uint8_t((uint16_t(tl.a) + uint16_t(tr.a) + uint16_t(bl.a) + uint16_t(br.a)) / 4));
			}
		}

		source = m_mipLevels.back();
	}
	while (source.getWidth() > 1 && source.getHeight() > 1);
	
}

void tr::ColorBuffer::copyTextureData(const std::vector<uint8_t>& decodedData)
{
	for (size_t y = 0, i = 0; y < m_height; ++y)
	{
		for (size_t x = 0; x < m_width; ++x, i += 4)
		{
			const uint8_t* const pixelData = &decodedData[i];

			at(x, y) = Color(
				*(pixelData + 2),
				*(pixelData + 1),
				*(pixelData + 0),
				*(pixelData + 3)
			);
		}
	}
}

bool tr::ColorBuffer::isPowerOfTwo(const size_t x)
{
    return (x != 0) && ((x & (x - 1)) == 0);
}
