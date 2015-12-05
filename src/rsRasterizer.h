#ifndef RS_RASTERIZER_H
#define RS_RASTERIZER_H

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

    void Draw( const rs::DrawMode mode, const vector<rs::Vec3>& vertices, const vector<size_t>& indices, const rs::Texture&  texture, const rs::Mat3& modelViewMatrix, const rs::Mat3& projectionMatrix, rs::FrameBuffer& buffer )
    {
        buffer.at(buffer.width/2, buffer.height/2) =
    }
}

#endif // RS_RASTERIZER_H
