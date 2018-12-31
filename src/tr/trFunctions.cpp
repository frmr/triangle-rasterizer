#include "trFunctions.hpp"
#include "trFileException.hpp"

#include "../EasyBMP/EasyBMP.h"

tr::ColorBuffer tr::loadTexture(const std::string& filename)
{
	BMP* image   = new BMP();
	bool success = image->ReadFromFile(filename.c_str());

	if (success)
	{
		ColorBuffer colorBuffer(image->TellWidth(), image->TellHeight());

		for (int x = 0; x < image->TellWidth(); ++x)
		{
			for (int y = 0; y < image->TellHeight(); ++y)
			{
				colorBuffer.at(x, y) =  ((Color) image->GetPixel(x, y).Red)   << 16;
				colorBuffer.at(x, y) += ((Color) image->GetPixel(x, y).Green) << 8;
				colorBuffer.at(x, y) += ((Color) image->GetPixel(x, y).Blue)  << 0;
			}
		}

		delete image;

		return colorBuffer;
	}
	else
	{
		delete image;

		throw FileException("Failed to load texture " + filename);
	}
}
