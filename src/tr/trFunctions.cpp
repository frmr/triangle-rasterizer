#include "trFunctions.hpp"
#include "trFileException.hpp"

#include "../lodepng/lodepng.h"

tr::ColorBuffer tr::loadTexture(const std::string& filename)
{
	std::vector<unsigned char> encodedData;
	std::vector<unsigned char> decodedData;
	unsigned int               width;
	unsigned int               height;
	lodepng::State             state;

	state.info_raw.colortype = LodePNGColorType::LCT_RGBA;

	lodepng::load_file(encodedData, filename);
	lodepng::decode(decodedData, width, height, state, encodedData);

	ColorBuffer colorBuffer(width, height);

	for (unsigned int y = 0, i = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x, i += 4)
		{
			Color&         color     = colorBuffer.at(x, y);
			unsigned char* pixelData = &decodedData[i];
	
			color.red   = *(pixelData    );
			color.green = *(pixelData + 1);
			color.blue  = *(pixelData + 2);
			color.alpha = *(pixelData + 3);
		}
	}

	return colorBuffer;
}
