//
// Created by cpasjuste on 30/05/23.
//

#include <cstdio>
#include "display_linux.h"

using namespace mb;

LinuxDisplay::LinuxDisplay() : Display() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s", SDL_GetError());
        return;
    }

    p_window = SDL_CreateWindow("MicroBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                m_size.x, m_size.y, SDL_WINDOW_SHOWN);
    if (!p_window) {
        fprintf(stderr, "SDL_CreateWindow error: %s", SDL_GetError());
        return;
    }

    p_renderer = SDL_CreateRenderer(p_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!p_renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s", SDL_GetError());
        return;
    }

    printf("LinuxDisplay: %ix%i\n", m_size.x, m_size.y);
}

void LinuxDisplay::drawPixel(const Utility::Vec2i &pos, uint16_t pixel) {
    if (pos.x > m_size.x || pos.y > m_size.y) return;
    SDL_SetRenderDrawColor(p_renderer, pixel & 0xff, (pixel >> 5) & 0xff, (pixel >> 10) & 0xff, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawPoint(p_renderer, pos.x, pos.y);
}

void LinuxDisplay::clear() {
    SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);
    SDL_RenderClear(p_renderer);
}

void LinuxDisplay::flip() {
    SDL_RenderPresent(p_renderer);
}

LinuxDisplay::~LinuxDisplay() {
    printf("~LinuxDisplay()\n");
    SDL_DestroyRenderer(p_renderer);
    SDL_DestroyWindow(p_window);
}
