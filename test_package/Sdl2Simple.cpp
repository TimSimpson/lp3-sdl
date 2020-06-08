// --------------------------------------------------------------------
// This example shows using `lp3::sdl`'s helper classes to create
// a simple app.
// --------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <lp3/main.hpp>
#include <lp3/sdl.hpp>
#include <memory>
#include <SDL_image.h>

namespace sdl = lp3::sdl;

int _main(lp3::main::PlatformLoop & loop) {
    sdl::SDL2 sdl2(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Log("Hello SDL!");

    sdl::Window window = SDL_CreateWindow(
            "SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_Log("Creating renderer...");
    sdl::Renderer renderer
            = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Log("Loading texture...");

#if defined(__EMSCRIPTEN__)
    const std::string file("Earth.png");
#else
    if (loop.command_line_args().size() < 2) {
        SDL_Log("Expected picture for arg 2");
        return 1;
    }
    const std::string file = loop.command_line_args()[1];
#endif

    SDL_RWops * ptr = SDL_RWFromFile(file.c_str(), "rb");
    if (!ptr) {
        SDL_Log("Error opening file %s.", file.c_str());
        SDL_Log("SDL error %s.", SDL_GetError());
        return 1;
    }

    sdl::RWops png_file{ptr};
    sdl::Surface bitmap = IMG_LoadTyped_RW(png_file, 0, "PNG");

    // Here's an alternative way to load the bitmap:
    // const auto bmp_file = media.path("Engine/Gfx/Earth.bmp");
    // sdl::Surface bitmap = SDL_LoadBMP(bmp_file.c_str());

    sdl::Texture tex = SDL_CreateTextureFromSurface(renderer, bitmap);

    SDL_Log("start itr");
    loop.run([&renderer, &tex]() {
        SDL_Event e;
        bool quit = false;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, tex, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        return !quit;
    });

    return 0;
}

LP3_MAIN(_main)
// ~end-doc
