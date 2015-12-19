#include <chrono>
#include <iostream>
#include <vector>

#include "rs/rsRasterizer.h"
#include "rs/rsVec3.h"

#include <SDL.h>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

void CopyFrameBufferToSurface(rs::FrameBuffer& fb, SDL_Surface* sdlSurface)
{
    Uint32* pixels = (Uint32*)(sdlSurface->pixels);
    rs::Color* colorData = fb.GetColorData();

    for (unsigned int x = 0; x < fb.width; ++x)
    {
        for (unsigned int y = 0; y < fb.height; ++y)
        {
            const size_t index = y * fb.width + x;
            pixels[index] = (Uint32) ( colorData[index].r * 255.0 ) << 16;
            pixels[index] += (Uint32) ( colorData[index].g * 255.0 ) << 8;
            pixels[index] += (Uint32) ( colorData[index].b * 255.0 ) << 0;
        }
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

bool InitWindow(SDL_Window** window, SDL_Renderer** renderer, const int screenWidth, const int screenHeight)
{
    *window = SDL_CreateWindow("Space Raster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    //*window = SDL_CreateWindow( "gs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN );

    if (window == NULL)
    {
        cerr << "InitWindow() in src/main.cpp: Failed to initialise SDL window." << endl;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

    if ( renderer == NULL )
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

    const int screenWidth = 1024;
    const int screenHeight = 768;

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!InitWindow(&window, &renderer, screenWidth, screenHeight))
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    vector<rs::Vec3d> vertices;
    vector<size_t> indices;
    rs::Texture tex("");
    double* matrix;
    rs::FrameBuffer fb( 1024, 768 );


    SDL_Surface* sdlSurface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 0, 0, 0, 0);

    while (running)
    {
        std::chrono::time_point<std::chrono::system_clock> start;
        start = std::chrono::system_clock::now();

        SDL_Event e;
        while( SDL_PollEvent(&e) != 0 )
        {
            //User requests quit
            if(e.type == SDL_QUIT)
            {
                running = false;
            }
        }

        rs::Draw(rs::DrawMode::POINTS, vertices, indices, tex, matrix, matrix, fb);
        CopyFrameBufferToSurface(fb, sdlSurface);

        SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, sdlSurface);
        SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(sdlTexture);


        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        cout << elapsed_seconds.count() * 1000 << "ms" << endl;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
