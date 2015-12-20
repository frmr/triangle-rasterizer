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
        const size_t totalPixels = fb.width * fb.height;
        for (size_t index = 0; index < totalPixels; ++index)
        {
            fb.colorBuffer.data[index] = texture.colorBuffer.data[index];
        }
    }
}

#endif // RS_RASTERIZER_H
