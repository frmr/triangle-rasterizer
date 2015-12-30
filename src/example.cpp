#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "rs/rsMat4.h"
#include "rs/rsRasterizer.h"
#include "rs/rsVec4.h"

#include <SDL.h>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

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

    vector<rs::Vec4d> vertices;
    vertices.emplace_back(0.0, 0.0, -10.0, 1.0);

    vector<size_t> indices;

    rs::Texture tex("data/udon1.bmp");

    rs::Mat4d modelViewMatrix;
    modelViewMatrix.SetIdentity();
    modelViewMatrix.RotateX(3.1416 / 4.0);
    modelViewMatrix.RotateY(3.1416 / 4.0);

    rs::Mat4d projectionMatrix;

    //projectionMatrix.SetOrthographic(-1.0, 1.0, -1.0, 1.0, 0.0, -100.0);
    projectionMatrix.SetPerspective(-1.0, 1.0, -1.0, 1.0, -1.0, -100.0);


    rs::FrameBuffer fb(screenWidth, screenHeight);

    SDL_Surface* sdlSurface;

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
        }

        rs::Draw(rs::DrawMode::POINTS, vertices, indices, tex, modelViewMatrix, projectionMatrix, fb);
        sdlSurface = SDL_CreateRGBSurfaceFrom((void*) fb.colorBuffer.data, screenWidth, screenHeight, 32, sizeof(rs::Color)*screenWidth, 0, 0, 0, 0);

        SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, sdlSurface);
        SDL_FreeSurface(sdlSurface);
        SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
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
