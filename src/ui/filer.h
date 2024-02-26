//
// Created by cpasjuste on 14/06/23.
//

#ifndef MICROBOY_FILER_H
#define MICROBOY_FILER_H

#include <vector>
#include "widget.h"
#include "text.h"
#include "../cores/core.h"

#define ROMS_FOLDER_COUNT 4

namespace mb {
    class Filer : public p2d::Widget {
    public:
        explicit Filer(const p2d::Utility::Vec2i &pos, const p2d::Utility::Vec2i &size);

        void load();

        void setCore(const Core::Type &core);

        Core::Type getCore() { return m_core; }

        [[nodiscard]] bool isDone() const { return m_done; }

        bool isEmpty() {
            return getListBuffer(Core::Type::Nes)->count < 1
                   && getListBuffer(Core::Type::Gb)->count < 1
                   && getListBuffer(Core::Type::Sms)->count < 1
                   && getListBuffer(Core::Type::Gg)->count < 1;
        }

    private:
        p2d::Platform *p_platform;
        Core::Type m_core = Core::Type::Nes;
        int m_max_lines = 0;
        int m_line_height = 0;
        int m_file_index = 0;
        int m_highlight_index = 0;
        p2d::Rectangle *p_highlight;
        std::vector<p2d::Text *> p_lines;
        p2d::Text *p_no_rom_text;
        bool m_done = false;

        bool onInput(const uint16_t &buttons) override;

        void refresh();

        void setSelection(int index);

        p2d::Io::ListBuffer *getListBuffer(uint8_t index);
    };
}

#endif //MICROBOY_FILER_H
