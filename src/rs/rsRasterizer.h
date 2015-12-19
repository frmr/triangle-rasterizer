#ifndef RS_RASTERIZER_H
#define RS_RASTERIZER_H

#include <vector>

#include "rsFrameBuffer.h"
#include "rsTexture.h"
#include "rsVec3.h"

using std::vector;

namespace rs
{
    enum class DrawMode
    {
        POINTS,
        LINES,
        LINE_STRIP,
        LINE_LOOP,
        TRIANGLES,
        TRIANGLE_STRIP,
        TRIANGLE_FAN
    };

    void Draw(const rs::DrawMode mode, const vector<rs::Vec3d>& vertices, const vector<size_t>& indices, const rs::Texture& texture, const double* const modelViewMatrix, const double* const projectionMatrix, rs::FrameBuffer& fb)
    {
        fb.ColorAt(fb.width/2, fb.height/2).r = 1.0;

        for (size_t x = 0; x < 400; ++x)
        {
            for (size_t y = 0; y < 400; ++y)
            {
                fb.ColorAt(x, y) = texture.GetAt(x, y);
            }
        }
    }
}

#endif // RS_RASTERIZER_H
