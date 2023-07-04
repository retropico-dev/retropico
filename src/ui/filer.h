//
// Created by cpasjuste on 14/06/23.
//

#ifndef MICROBOY_FILER_H
#define MICROBOY_FILER_H

#include <vector>
#include "widget.h"
#include "text.h"

namespace mb {
    class Filer : public Widget {
    public:
        explicit Filer(const Utility::Vec2i &pos, const Utility::Vec2i &size);

        [[nodiscard]] bool isDone() const { return m_done; }

    private:
        Platform *p_platform;
        std::string m_rom;
        Io::FileListBuffer m_files;
        int m_max_lines = 0;
        int m_line_height = 0;
        int m_file_index = 0;
        int m_highlight_index = 0;
        Rectangle *p_highlight;
        std::vector<Text *> p_lines;
        bool m_done = false;

        void loop(const Utility::Vec2i &pos, const uint16_t &buttons) override;

        void setSelection(int index);
    };
}

#endif //MICROBOY_FILER_H
