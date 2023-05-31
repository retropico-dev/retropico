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

void LinuxDisplay::clear() {
    SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);
    SDL_RenderClear(p_renderer);
}

void LinuxDisplay::flip() {
    SDL_RenderPresent(p_renderer);
}

void LinuxDisplay::drawLine(uint8_t y, uint8_t width, uint16_t *line) {
    if (y >= m_size.y) return;

    for (int x = 0; x < m_size.x; x++) {
        if (x >= width) break;
        uint16_t p = line[x];
        SDL_SetRenderDrawColor(p_renderer, p & 0xff, (p >> 5) & 0xff, (p >> 10) & 0xff, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawPoint(p_renderer, x, y);
    }

    if (y == m_size.y - 1) {
        flip();
    }
}

LinuxDisplay::~LinuxDisplay() {
    printf("~LinuxDisplay()\n");
    SDL_DestroyRenderer(p_renderer);
    SDL_DestroyWindow(p_window);
}
