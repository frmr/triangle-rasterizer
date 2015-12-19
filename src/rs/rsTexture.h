#ifndef RS_TEXTURE_H
#define RS_TEXTURE_H

#include <iostream>
#include <string>

#include "../EasyBMP/EasyBMP.h"
#include "rsBuffer.h"
#include "rsColor.h"

using std::cerr;
using std::endl;
using std::string;

namespace rs
{
    class Texture
    {
    private:
        //image loaded from file in initialiser list, then deleted at the end of the constructor
        BMP* tempColorData;
        rs::Buffer<rs::Color> colorBuffer;

    private:
        static BMP* LoadTempColorData(const string& filename)
        {
            BMP* image = new BMP();
            bool success = image->ReadFromFile(filename.c_str());
            if (!success)
            {
                cerr << "rs::Texture::LoadTempColorData() in src/rs/Texture.h: Failed to load texture " << filename << "." << endl;
            }
            return image;
        }

    public:
        rs::Color& At(const size_t x, const size_t y)
        {
            return colorBuffer.At(x, y);
        }

        rs::Color& At(const double x, const double y, const bool filter)
        {
            return colorBuffer.At(x, y, filter);
        }

        rs::Color GetAt(const size_t x, const size_t y) const
        {
            return colorBuffer.GetAt(x, y);
        }

        rs::Color GetAt(const double x, const double y, const bool filter) const
        {
            return colorBuffer.GetAt(x, y, filter);
        }

    public:
        explicit Texture(const string& filename)
            :   tempColorData(LoadTempColorData(filename)),
                colorBuffer(tempColorData->TellWidth(), tempColorData->TellHeight())
        {
            for (int x = 0; x < tempColorData->TellWidth(); ++x)
            {
                for (int y = 0; y < tempColorData->TellHeight(); ++y)
                {
                    colorBuffer.At(x, y).r = tempColorData->GetPixel(x, y).Red;
                    colorBuffer.At(x, y).g = tempColorData->GetPixel(x, y).Green;
                    colorBuffer.At(x, y).b = tempColorData->GetPixel(x, y).Blue;
                }
            }
            delete tempColorData;
        }
    };
}

#endif // RS_TEXTURE_H
