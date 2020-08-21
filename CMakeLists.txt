cmake_minimum_required (VERSION 3.10)

add_library (triangle-rasterizer STATIC
	src/matrix/Matrices.cpp
	src/tr/trDefaultShader.cpp
	src/tr/trDepthShader.cpp
	src/tr/trTexture.cpp
	src/tr/trTransformedVertex.cpp
)