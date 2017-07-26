#include "trFrameBuffer.hpp"

tr::FrameBuffer::FrameBuffer(const size_t& width, const size_t& height) :
	width(width),
	height(height),
	color(width, height),
	depth(width, height)
{
}

tr::Color& tr::FrameBuffer::colorAt(const size_t& x, const size_t& y)
{
	return color.at(x, y);
}

float& tr::FrameBuffer::depthAt(const size_t& x, const size_t& y)
{
	return depth.at(x, y);
}