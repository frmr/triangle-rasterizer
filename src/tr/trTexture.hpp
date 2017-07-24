#pragma once

#include <iostream>
#include <string>

#include "../EasyBMP/EasyBMP.h"
#include "trBuffer.hpp"
#include "trColor.hpp"

using std::cerr;
using std::endl;
using std::string;

namespace tr
{
	class Texture
	{
	private:
		BMP* tempColorData;

	public:
		tr::Buffer<tr::Color> colorBuffer;

	private:
		static BMP* LoadTempColorData(const string& filename)
		{
			BMP* image = new BMP();
			bool success = image->ReadFromFile(filename.c_str());

			if (!success)
			{
				cerr << "tr::Texture::LoadTempColorData() in src/rs/Texture.h: Failed to load texture " << filename << "." << endl;
			}

			return image;
		}

	public:
		tr::Color& At(const size_t& x, const size_t& y)
		{
			return colorBuffer.At(x, y);
		}

		tr::Color& At(const double& x, const double& y, const bool filter)
		{
			return colorBuffer.At(x, y, filter);
		}

		tr::Color GetAt(const size_t& x, const size_t& y) const
		{
			return colorBuffer.GetAt(x, y);
		}

		tr::Color GetAt(const double& x, const double& y, const bool filter) const
		{
			return colorBuffer.GetAt(x, y, filter);
		}

		tr::Color GetAt(const size_t& index) const
		{
			return colorBuffer.data[index];
		}

	public:
		explicit Texture(const string& filename) :
			tempColorData(LoadTempColorData(filename)),
			colorBuffer(tempColorData->TellWidth(), tempColorData->TellHeight())
		{
			for (int x = 0; x < tempColorData->TellWidth(); ++x)
			{
				for (int y = 0; y < tempColorData->TellHeight(); ++y)
				{
					colorBuffer.At(x, y) = ((tr::Color) tempColorData->GetPixel(x, y).Red) << 16;
					colorBuffer.At(x, y) += ((tr::Color) tempColorData->GetPixel(x, y).Green) << 8;
					colorBuffer.At(x, y) += ((tr::Color) tempColorData->GetPixel(x, y).Blue) << 0;
				}
			}

			delete tempColorData;
		}
	};
}