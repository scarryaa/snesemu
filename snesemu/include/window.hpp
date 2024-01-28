#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SDL.h>
#include "../include/imgui/imgui.h"
#include "../include/imgui/imconfig.h"
#include "../include/imgui/imgui_internal.h"
#include "../include/imgui/imgui_impl_sdl2.h"
#include "../include/imgui/imgui_impl_sdlrenderer2.h"

class Emulator;

class Window
{
public:
    Window();
    ~Window();

    bool pollEvents();
    void render(Emulator* emulator);
    void postRender(uint8_t* frame_buffer);
    void renderDisassembly(Emulator* emulator);
    void renderCpuMemoryView(Emulator* emulator);
    void renderVramView(Emulator* emulator);
    void renderBreakpoints(Emulator* emulator);
    void renderCgramView(Emulator* emulator);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};

#endif