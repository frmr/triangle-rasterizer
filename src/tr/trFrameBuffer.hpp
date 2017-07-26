#pragma once

#include "trColorBuffer.hpp"
#include "trDepthBuffer.hpp"

namespace tr
{
	class FrameBuffer
	{
	public:
		FrameBuffer(const size_t& width, const size_t& height) :
			width(width),
			height(height),
			color(width, height),
			depth(width, height)
		{
		}

		tr::Color& ColorAt(const size_t& x, const size_t& y)
		{
			return color.At(x, y);
		}

		float& DepthAt(const size_t& x, const size_t& y)
		{
			return depth.At(x, y);
		}

	public:
		const size_t    width;
		const size_t    height;
		tr::ColorBuffer color;
		tr::DepthBuffer depth;
	};
}
