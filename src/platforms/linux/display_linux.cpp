//
// Created by cpasjuste on 30/05/23.
//

#include <cstdio>
#include "platform.h"

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

    printf("LinuxDisplay: %ix%i (texture pitch: %i)\n", m_size.x, m_size.y, m_pitch);
}

void LinuxDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (y < 0) || x > m_size.x || y > m_size.y) return;

    // rotation
    int16_t t;
    switch (rotation) {
        case 1:
            t = x;
            x = (int16_t) (m_size.x - 1 - y);
            y = t;
            break;
        case 2:
            x = (int16_t) (m_size.x - 1 - x);
            y = (int16_t) (m_size.y - 1 - y);
            break;
        case 3:
            t = x;
            x = y;
            y = (int16_t) (m_size.y - 1 - t);
            break;
    }

#if 1
    // rgb565 > rgb32
    int32_t r = ((color & 0xF800) >> 11) << 3;
    int32_t g = ((color & 0x7E0) >> 5) << 2;
    int32_t b = ((color & 0x1F)) << 3;
#else
    // rgb555 > rgb32
    int32_t r = ((color & 0x7C00) >> 10) << 3;
    int32_t g = ((color & 0x3E0) >> 5) << 3;
    int32_t b = (color & 0x1F) << 3;
#endif

    // draw the pixel to the renderer
    SDL_SetRenderDrawColor(p_renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawPoint(p_renderer, x, y);
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
