#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "tr/tr.hpp"

#include <SDL.h>

bool initSdl()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
	{
		std::cerr << "InitSdl() in src/main.cpp: Failed to initialise SDL." << std::endl;
		return false;
	}
	return true;
}

bool initWindow(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture, const int screenWidth, const int screenHeight, const bool fullscreen)
{
	*window = SDL_CreateWindow("Space Raster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));

	if (!window)
	{
		std::cerr << "InitWindow() in src/main.cpp: Failed to initialise SDL window." << std::endl;
		return false;
	}

	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer)
	{
		std::cerr << "InitWindow() in src/main.cpp: Failed to initialise SDL renderer." << std::endl;
	}

	*texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);

	if (!texture)
	{
		std::cerr << "InitWindow() in src/main.cpp: Failed to initialize SDL teuxtre." << std::endl;
	}

	return true;
}

Matrix4 createPerspectiveProjectionMatrix(const float left, const float right, const float bottom, const float top, const float near, const float far)
{
	Matrix4 mat;

	mat[0]  = (2.0f * near) / (right - left);
	mat[5]  = (2.0f * near) / (top - bottom);
	mat[8]  = (right + left) / (right - left);
	mat[9]  = (top + bottom) / (top - bottom);
	mat[10] = -(far + near) / (far - near);
	mat[11] = -1.0f;
	mat[14] = -(2.0f * far * near) / (far - near);

	return mat;
}

Matrix4 createOrthographicProjectionMatrix(const float left, const float right, const float bottom, const float top, const float near, const float far)
{
	Matrix4 mat;

	mat[0]  = 2.0f / (right - left);
	mat[5]  = 2.0f / (top - bottom);
	mat[10] = -2.0f / (far - near);
	mat[12] = -(right + left) / (right - left);
	mat[13] = -(top + bottom) / (top - bottom);
	mat[14] = -(far + near) / (far - near);

	return mat;
}

std::vector<tr::Vertex> defineVertices()
{
	return {
		
		// Front
		tr::Vertex(Vector4( 2.0f,  2.0f,  2.0f, 1.0f), Vector3(0.0f, 0.0f,  1.0f), Vector2(2.0f, 0.0f)),
		tr::Vertex(Vector4( 2.0f, -2.0f,  2.0f, 1.0f), Vector3(0.0f, 0.0f,  1.0f), Vector2(2.0f, 2.0f)),
		tr::Vertex(Vector4(-2.0f, -2.0f,  2.0f, 1.0f), Vector3(0.0f, 0.0f,  1.0f), Vector2(0.0f, 2.0f)),

		tr::Vertex(Vector4(-2.0f,  2.0f,  2.0f, 1.0f), Vector3(0.0f, 0.0f,  1.0f), Vector2(0.0f, 0.0f)),
		tr::Vertex(Vector4( 2.0f,  2.0f,  2.0f, 1.0f), Vector3(0.0f, 0.0f,  1.0f), Vector2(2.0f, 0.0f)),
		tr::Vertex(Vector4(-2.0f, -2.0f,  2.0f, 1.0f), Vector3(0.0f, 0.0f,  1.0f), Vector2(0.0f, 2.0f)),

		// Top
		tr::Vertex(Vector4(-2.0f,  2.0f,  2.0f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(0.0f, 0.0f)),
		tr::Vertex(Vector4(-2.0f,  2.0f, -2.0f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(1.0f, 0.0f)),
		tr::Vertex(Vector4( 2.0f,  2.0f,  2.0f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(0.0f, 1.0f)),

		tr::Vertex(Vector4(-2.0f,  2.0f, -2.0f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(1.0f, 0.0f)),
		tr::Vertex(Vector4( 2.0f,  2.0f, -2.0f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(1.0f, 1.0f)),
		tr::Vertex(Vector4( 2.0f,  2.0f,  2.0f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(0.0f, 1.0f)),

		// Right
		tr::Vertex(Vector4( 2.0f,  2.0f, -2.0f, 1.0f), Vector3(1.0f, 0.0f,  0.0f), Vector2(1.0f, 0.0f)),
		tr::Vertex(Vector4( 2.0f, -2.0f, -2.0f, 1.0f), Vector3(1.0f, 0.0f,  0.0f), Vector2(1.0f, 1.0f)),
		tr::Vertex(Vector4( 2.0f,  2.0f,  2.0f, 1.0f), Vector3(1.0f, 0.0f,  0.0f), Vector2(0.0f, 0.0f)),

		tr::Vertex(Vector4( 2.0f,  2.0f,  2.0f, 1.0f), Vector3(1.0f, 0.0f,  0.0f), Vector2(0.0f, 0.0f)),
		tr::Vertex(Vector4( 2.0f, -2.0f, -2.0f, 1.0f), Vector3(1.0f, 0.0f,  0.0f), Vector2(1.0f, 1.0f)),
		tr::Vertex(Vector4( 2.0f, -2.0f,  2.0f, 1.0f), Vector3(1.0f, 0.0f,  0.0f), Vector2(0.0f, 1.0f)),

		// Back
		tr::Vertex(Vector4(-2.0f, -2.0f, -2.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f)),
		tr::Vertex(Vector4( 2.0f, -2.0f, -2.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f)),
		tr::Vertex(Vector4( 2.0f,  2.0f, -2.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f)),
		
		tr::Vertex(Vector4(-2.0f, -2.0f, -2.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f)),
		tr::Vertex(Vector4( 2.0f,  2.0f, -2.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f)),
		tr::Vertex(Vector4(-2.0f,  2.0f, -2.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f)),
	};
}

void renderColorBufferToWindow(tr::ColorBuffer& colorBuffer, SDL_Renderer* renderer, SDL_Texture* texture)
{
	tr::Color* pixels;
	int        pitch;
	
	SDL_SetRenderTarget(renderer, texture);
	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
	
	memcpy(pixels, colorBuffer.getData(), colorBuffer.getWidth() * colorBuffer.getHeight() * 4);
	
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void updateInputs(bool& running, Vector4& position, Vector4& rotation)
{
	SDL_Event  event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			running = false;
		}
		else if (event.type == SDL_KEYDOWN)
		{
			constexpr float translationIncrement = 0.1f;
			constexpr float rotationIncrement    = 2.0f;

			if      (event.key.keysym.sym == SDLK_ESCAPE) { running = false;                    }
			else if (event.key.keysym.sym == SDLK_w)      { position.z -= translationIncrement; }
			else if (event.key.keysym.sym == SDLK_a)      { position.x -= translationIncrement; }
			else if (event.key.keysym.sym == SDLK_s)      { position.z += translationIncrement; }
			else if (event.key.keysym.sym == SDLK_d)      { position.x += translationIncrement; }
			else if (event.key.keysym.sym == SDLK_SPACE)  { position.y += translationIncrement; }
			else if (event.key.keysym.sym == SDLK_LCTRL)  { position.y -= translationIncrement; }
			else if (event.key.keysym.sym == SDLK_LEFT)   { rotation.y += rotationIncrement;    }
			else if (event.key.keysym.sym == SDLK_RIGHT)  { rotation.y -= rotationIncrement;    }
			else if (event.key.keysym.sym == SDLK_UP)     { rotation.x += rotationIncrement;    }
			else if (event.key.keysym.sym == SDLK_DOWN)   { rotation.x -= rotationIncrement;    }
		}
	}
}

int main(int argc, char* argv[])
{
	constexpr int                     screenWidth      = 1920;
	constexpr int                     screenHeight     = 1080;
	bool                              running          = true;
	SDL_Window*                       sdlWindow        = nullptr;
	SDL_Renderer*                     sdlRenderer      = nullptr;
	SDL_Texture*                      sdlTexture       = nullptr;
	const std::vector<tr::Vertex>     vertices         = defineVertices();
	const float                       aspectRatio      = float(screenWidth) / float(screenHeight);
	const Matrix4                     projectionMatrix = createPerspectiveProjectionMatrix(-aspectRatio, aspectRatio, -1.0f, 1.0f, 1.0f, 100.0f);
	const size_t                      interlaceGap     = 1;
	size_t                            interlaceOffset  = 0;
	const tr::ColorBuffer             texture("data/udon.png");
	tr::ColorBuffer                   colorBuffer(screenWidth, screenHeight);
	tr::DepthBuffer                   depthBuffer(screenWidth, screenHeight);
	tr::DefaultShader                 shader;

	Vector4                           cameraRotation(0.0f, 0.0f, 0.0f, 1.0f);
	Vector4                           cameraPosition(0.0f, 0.0f, 10.0f, 1.0f);

	tr::Rasterizer<tr::DefaultShader> rasterizer;

	if (screenWidth <= 0 || screenHeight <= 0)
	{
		return 0;
	}

	if (!initSdl())
	{
		SDL_Quit();
		return 0;
	}

	if (!initWindow(&sdlWindow, &sdlRenderer, &sdlTexture, screenWidth, screenHeight, false))
	{
		SDL_DestroyRenderer(sdlRenderer);
		SDL_DestroyWindow(sdlWindow);
		SDL_Quit();

		return 0;
	}

	rasterizer.setPrimitive(tr::Primitive::Triangles);
	rasterizer.setDepthTest(true);
	rasterizer.setTextureMode(tr::TextureMode::Perspective);
	rasterizer.setCullFaceMode(tr::CullFaceMode::None);
	rasterizer.setInterlace(interlaceOffset, interlaceGap);

	shader.setTexture(&texture);
	shader.setTextureWrappingMode(tr::TextureWrappingMode::Repeat);
	shader.setTextureFiltering(false);
	shader.setBlendMode(tr::BlendMode::None);

	double cumulativeFrameTime = 0.0;
	int    frameCount          = 0;

	while (running)
	{
		const auto start = std::chrono::high_resolution_clock::now();
		Matrix4    viewMatrix;
		
		updateInputs(running, cameraPosition, cameraRotation);

		colorBuffer.fill(tr::Color(0, 0, 0, 255));
		depthBuffer.fill(1.0f);
		
		viewMatrix.identity();
		viewMatrix.translate(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z);
		viewMatrix.rotateY(-cameraRotation.y);
		viewMatrix.rotateX(-cameraRotation.x);
		
		rasterizer.setInterlace(interlaceOffset, interlaceGap);

		if (++interlaceOffset == interlaceGap)
		{
			interlaceOffset = 0;
		}
		
		rasterizer.setMatrix(projectionMatrix * viewMatrix);
		rasterizer.draw(vertices, shader, colorBuffer, depthBuffer);

		renderColorBufferToWindow(colorBuffer, sdlRenderer, sdlTexture);

		const auto frameTime = std::chrono::high_resolution_clock::now() - start;

		cumulativeFrameTime += std::chrono::duration<double, std::milli>(frameTime).count();
		++frameCount;

		if (frameCount == 50)
		{
			std::cout << cumulativeFrameTime / frameCount << "ms" << std::endl;

			cumulativeFrameTime = 0.0;
			frameCount          = 0;
		}
	}

	SDL_DestroyTexture(sdlTexture);
	SDL_DestroyRenderer(sdlRenderer);
	SDL_DestroyWindow(sdlWindow);
	SDL_Quit();

	return 0;
}
