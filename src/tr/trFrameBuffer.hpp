#pragma once

#include "trColorBuffer.hpp"
#include "trDepthBuffer.hpp"

namespace tr
{
	class FrameBuffer
	{
	public:
		FrameBuffer(const size_t& width, const size_t& height);

		tr::Color& colorAt(const size_t& x, const size_t& y);
		float&     depthAt(const size_t& x, const size_t& y);

	public:
		const size_t    width;
		const size_t    height;
		tr::ColorBuffer color;
		tr::DepthBuffer depth;
	};
}
