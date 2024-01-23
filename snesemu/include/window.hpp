#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SDL.h>

class Emulator;

class Window
{
public:
    Window();
    ~Window();

    bool poll_events();
    void render(Emulator* emulator);
    void post_render(uint8_t* frame_buffer);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};

#endif