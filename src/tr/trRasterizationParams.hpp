#pragma once

#include "trTextureMode.hpp"

namespace tr
{
	struct RasterizationParams
	{
		bool        depthTest;
		float       depthBias;
		TextureMode textureMode;
	};
}