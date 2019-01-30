#include "trColorBuffer.hpp"

#include "../lodepng/lodepng.h"

tr::ColorBuffer::ColorBuffer(const std::string& filename) :
	Buffer<Color>()
{
	std::vector<unsigned char> encodedData;

	if (!lodepng::load_file(encodedData, filename))
	{
		unsigned int               width;
		unsigned int               height;
		lodepng::State             state;
		std::vector<unsigned char> decodedData;

		state.info_raw.colortype = LodePNGColorType::LCT_RGBA;

		if (!lodepng::decode(decodedData, width, height, state, encodedData))
		{
			init(width, height);
			copyTextureData(decodedData);

			return;
		}
	}
	
	init(8, 8);
	generatePlaceholder();
}

void tr::ColorBuffer::copyTextureData(const std::vector<unsigned char>& decodedData)
{
	for (unsigned int y = 0, i = 0; y < m_height; ++y)
	{
		for (unsigned int x = 0; x < m_width; ++x, i += 4)
		{
			const unsigned char* const pixelData = &decodedData[i];

			at(x, y) = Color(
				*(pixelData + 2),
				*(pixelData + 1),
				*(pixelData + 0),
				*(pixelData + 3)
			);
		}
	}
}

void tr::ColorBuffer::generatePlaceholder()
{
	bool purple = false;

	for (unsigned int y = 0; y < m_height; ++y, purple = !purple)
	{
		for (unsigned int x = 0; x < m_width; ++x, purple = !purple)
		{
			at(x, y) = Color(
				purple ? 255 : 0,
				0,
				purple ? 255 : 0,
				255
			);
		}
	}
}
