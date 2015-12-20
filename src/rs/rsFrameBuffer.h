#ifndef RS_FRAMEBUFFER_H
#define RS_FRAMEBUFFER_H

#include "rsBuffer.h"
#include "rsColor.h"

namespace rs
{
    class FrameBuffer
    {
    public:
        const size_t            width;
        const size_t            height;
        rs::Buffer<rs::Color>   colorBuffer;
        rs::Buffer<double>      depthBuffer;

    public:
        rs::Color& ColorAt(const size_t& x, const size_t& y)
        {
            return colorBuffer.At(x, y);
        }

        double& DepthAt(const size_t& x, const size_t& y)
        {
            return depthBuffer.At(x, y);
        }

    public:
        FrameBuffer(const size_t& width, const size_t& height)
            :   width(width),
                height(height),
                colorBuffer(width, height),
                depthBuffer(width, height)
        {
        }
    };
}

#endif // RS_FRAMEBUFFER_H
