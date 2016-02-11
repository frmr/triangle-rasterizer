#ifndef TR_FRAMEBUFFER_HPP
#define TR_FRAMEBUFFER_HPP

#include "trBuffer.hpp"
#include "trColor.hpp"

namespace tr
{
    class FrameBuffer
    {
    public:
        const size_t            width;
        const size_t            height;
        tr::Buffer<tr::Color>   colorBuffer;
        tr::Buffer<double>      depthBuffer;

    public:
        tr::Color& ColorAt(const size_t& x, const size_t& y)
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

#endif
