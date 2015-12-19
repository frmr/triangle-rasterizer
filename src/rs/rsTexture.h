#ifndef RS_TEXTURE_H
#define RS_TEXTURE_H

#include <string>

#include "rsBuffer.h"
#include "rsColor.h"

using std::string;

namespace rs
{
    class Texture
    {
    private:
        rs::Buffer<rs::Color> colorBuffer;
        //image loaded from file in initialiser list, then deleted at the end of the constructor

    public:
        rs::Color& Sample(const size_t x, const size_t y)
        {
            return colorBuffer.Sample(x, y);
        }

        rs::Color& Sample(const double x, const double y, const bool filter)
        {
            return colorBuffer.Sample(x, y, filter);
        }

    public:
        Texture(const string& filename)
            :   //image(filename),
                colorBuffer(1024, 1024)
                //colorBuffer(image.width, image.height)

        {
            //load file
            //note width and height
            //fill data
            colorBuffer.Sample(0, 0).r = 1.0;
        }
    };
}

#endif // RS_TEXTURE_H
