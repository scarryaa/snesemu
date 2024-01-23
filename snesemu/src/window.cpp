#include "../include/window.hpp"
#include "../include/emulator.hpp"

Window::Window() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }

    window = SDL_CreateWindow("espnes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);

    // Make the window resizable
    SDL_SetWindowResizable(this->window, SDL_TRUE);
}

Window::~Window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Window::poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return true;
        }
    }

    return false;
}

void Window::render(Emulator* emulator) {
}

void Window::post_render(uint8_t* frame_buffer) {
    // // Delay to control frame rate (16 ms = 60 fps)
    // SDL_Delay(16);

    SDL_RenderClear(this->renderer);
    SDL_UpdateTexture(this->texture, nullptr, frame_buffer, 256 * 4);
    SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
    SDL_RenderPresent(this->renderer);
}