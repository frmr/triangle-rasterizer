#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "rs/rsRasterizer.h"
#include "rs/rsVec3.h"

#include <SDL.h>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

void CopyFrameBufferSectionToSurface(const rs::FrameBuffer& fb, Uint32* pixels, const size_t iStart, const size_t iEnd)
{
    rs::Color* colorData = fb.GetColorData();
    for (size_t i = iStart; i < iEnd; ++i)
    {
        pixels[i] = ((Uint32) colorData[i].r) << 16;
        pixels[i] += ((Uint32) colorData[i].g) << 8;
        pixels[i] += ((Uint32) colorData[i].b) << 0;
    }
}

void CopyFrameBufferToSurface(const rs::FrameBuffer& fb, SDL_Surface* sdlSurface, const unsigned int numThreads)
{
    Uint32* pixels = (Uint32*)(sdlSurface->pixels);
    const size_t numPixels = fb.width * fb.height;

    vector<std::thread> threads;
    threads.reserve(numThreads);
    for (unsigned int t = 0; t < numThreads; ++t)
	{
		threads.push_back(std::thread(CopyFrameBufferSectionToSurface, std::cref(fb), pixels, t*numPixels/numThreads, (t+1)*numPixels/numThreads));
	}

	for (auto it = threads.begin(); it != threads.end(); ++it)
	{
		it->join();
	}
}

bool InitSdl()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        cerr << "InitSdl() in src/main.cpp: Failed to initialise SDL." << endl;
        return false;
    }
    return true;
}

bool InitWindow(SDL_Window** window, SDL_Renderer** renderer, const int screenWidth, const int screenHeight, const bool fullscreen)
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

int main(int argc, char* argv[])
{

    bool running = true;

    if (!InitSdl())
    {
        SDL_Quit();
        return 0;
    }

    const size_t screenWidth = 800;
    const size_t screenHeight = 600;

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!InitWindow(&window, &renderer, screenWidth, screenHeight, false))
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    vector<rs::Vec3d> vertices;
    vector<size_t> indices;
    rs::Texture tex("data/udon.bmp");
    double* matrix;
    rs::FrameBuffer fb(screenWidth, screenHeight);

    SDL_Surface* sdlSurface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 0, 0, 0, 0);

    while (running)
    {
        auto start = std::chrono::high_resolution_clock::now();

        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }
        }

        rs::Draw(rs::DrawMode::POINTS, vertices, indices, tex, matrix, matrix, fb);
        CopyFrameBufferToSurface(fb, sdlSurface, 4);

        SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, sdlSurface);
        SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(sdlTexture);

        auto end = std::chrono::high_resolution_clock::now();
        auto diff = end - start;
        cout << std::chrono::duration<double, std::milli>(diff).count() << " ms" << endl;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
