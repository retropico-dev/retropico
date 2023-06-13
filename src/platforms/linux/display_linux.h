//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_DISPLAY_LINUX_H
#define MICROBOY_DISPLAY_LINUX_H

// linux sdl2 display interface

namespace mb {
    class LinuxDisplay : public Display {
    public:
        LinuxDisplay();

        ~LinuxDisplay() override;

        void setCursor(uint16_t x, uint16_t y) override;

        void setPixel(uint16_t color) override;

        void flip() override;

    private:
        SDL_Window *p_window = nullptr;
        SDL_Renderer *p_renderer = nullptr;
        Utility::Vec2i m_cursor{};
    };
}

#endif //MICROBOY_DISPLAY_LINUX_H
