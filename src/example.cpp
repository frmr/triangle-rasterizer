#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "tr/tr.hpp"

#include <SDL.h>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

bool initSdl()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
	{
		cerr << "InitSdl() in src/main.cpp: Failed to initialise SDL." << endl;
		return false;
	}
	return true;
}

bool initWindow(SDL_Window** window, SDL_Renderer** renderer, const int screenWidth, const int screenHeight, const bool fullscreen)
{
	*window = SDL_CreateWindow("Space Raster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));

	if (!window)
	{
		cerr << "InitWindow() in src/main.cpp: Failed to initialise SDL window." << endl;
		return false;
	}

	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer)
	{
		cerr << "InitWindow() in src/main.cpp: Failed to initialise SDL renderer." << endl;
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

int main(int argc, char* argv[])
{
	bool running = true;

	if (!initSdl())
	{
		SDL_Quit();
		return 0;
	}

	const int screenWidth  = 800;
	const int screenHeight = 600;

	assert(screenWidth > 0 && screenHeight > 0);

	SDL_Window*   window   = NULL;
	SDL_Renderer* renderer = NULL;

	if (!initWindow(&window, &renderer, screenWidth, screenHeight, false))
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();

		return 0;
	}

	vector<Vector4> vertices;
	//vertices.emplace_back(0.0, 0.0, -1000.0, 1.0);
	vertices.emplace_back(2.0f,  2.0f,  10.0f, 1.0f);
	vertices.emplace_back(2.0f,  -2.0f, 10.0f, 1.0f);
	vertices.emplace_back(-2.0f, -2.0f, 10.0f, 1.0f);
	vertices.emplace_back(-2.0f, 2.0f,  10.0f, 1.0f);

	vector<size_t> indices = { 0, 1, 2, 3 };

	tr::ColorBuffer tex = tr::loadTexture("data/udon1.bmp");

	Matrix4 projectionMatrix = createPerspectiveProjectionMatrix(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);

	tr::ColorBuffer colorBuffer(screenWidth, screenHeight);
	tr::DepthBuffer depthBuffer(screenWidth, screenHeight);

	SDL_Surface* sdlSurface;

	Vector4 rotation(0.0f, 0.0f, 0.0f, 1.0f);
	Vector4 position(0.0f, 0.0f, 0.0f, 1.0f);

	Matrix4 modelViewMatrix;

	while (running)
	{
		const auto start = std::chrono::high_resolution_clock::now();

		SDL_Event e;

		while(SDL_PollEvent(&e))
		{
			if(e.type == SDL_QUIT)
			{
				running = false;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				constexpr float translationIncrement = 10.0f;
				constexpr float rotationIncrement    = 2.0f;

				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
				}
				else if (e.key.keysym.sym == SDLK_w)
				{
					position.z += translationIncrement;
				}
				else if (e.key.keysym.sym == SDLK_a)
				{
					position.x -= translationIncrement;
				}
				else if (e.key.keysym.sym == SDLK_s)
				{
					position.z -= translationIncrement;
				}
				else if (e.key.keysym.sym == SDLK_d)
				{
					position.x += translationIncrement;
				}
				else if (e.key.keysym.sym == SDLK_LSHIFT)
				{
					position.y += translationIncrement;
				}
				else if (e.key.keysym.sym == SDLK_LCTRL)
				{
					position.y -= translationIncrement;
				}
				else if (e.key.keysym.sym == SDLK_LEFT)
				{
					rotation.y += rotationIncrement;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					rotation.y -= rotationIncrement;
				}
				else if (e.key.keysym.sym == SDLK_UP)
				{
					rotation.x -= rotationIncrement;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					rotation.x += rotationIncrement;
				}
			}
		}

		std::cout << rotation.y << std::endl;

		colorBuffer.fill(0);

		modelViewMatrix.identity();
		modelViewMatrix.rotateX(rotation.x);
		modelViewMatrix.rotateY(rotation.y);
		modelViewMatrix.translate(-position.x, -position.y, -position.z);

		tr::draw(tr::DrawMode::LINE_LOOP, vertices, indices, tex, (modelViewMatrix * projectionMatrix).invert(), screenWidth, screenHeight, colorBuffer, depthBuffer);
		sdlSurface = SDL_CreateRGBSurfaceFrom((void*) colorBuffer.getData(), screenWidth, screenHeight, 32, sizeof(tr::Color) * screenWidth, 0, 0, 0, 0);

		SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, sdlSurface);
		SDL_FreeSurface(sdlSurface);
		SDL_RenderCopy(renderer, sdlTexture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(sdlTexture);

		const auto end = std::chrono::high_resolution_clock::now();
		const auto diff = end - start;
		//cout << std::chrono::duration<double, std::milli>(diff).count() << " ms" << endl;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
