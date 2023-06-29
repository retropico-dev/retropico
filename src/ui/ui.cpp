//
// Created by cpasjuste on 14/06/23.
//

#include "platform.h"
#include "ui.h"

#include "Fonts/FreeSans9pt7b.h"

using namespace mb;

//...
mb::Platform *s_platform;

Ui::Ui(Platform *platform) {
    p_platform = s_platform = platform;
    p_platform->getDisplay()->setFont(&FreeSans9pt7b);
    p_platform->getDisplay()->setTextSize(1);
    p_platform->getDisplay()->setTextWrap(false);
    m_files = p_platform->getIo()->getDir(Io::getRomPath());

    uint8_t display_height = platform->getDisplay()->getSize().y - 4;
    m_line_height = UI_FONT_HEIGHT + 4; // font height + margin
    m_max_lines = (int16_t) (display_height / m_line_height);
    if (m_max_lines * m_line_height < display_height) {
        m_line_height = (int16_t) (display_height / m_max_lines);
    }

    // set repeat delay for ui
    p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);

    flip();
}

void Ui::setSelection(int index) {
    if (index < m_max_lines / 2) {
        m_file_index = 0;
        m_highlight_index = 0;
    } else if (index > m_files.count - m_max_lines / 2) {
        m_highlight_index = m_max_lines / 2;
        m_file_index = m_files.count - 1 - m_highlight_index;
        if (m_highlight_index >= m_files.count) {
            m_highlight_index = m_files.count - 1;
            m_file_index = 0;
        }
    } else {
        m_highlight_index = m_max_lines / 2;
        m_file_index = index - m_highlight_index;
    }
    //flip();
}

bool Ui::loop() {
    // handle inputs
    uint16_t buttons = p_platform->getInput()->getButtons();

    // exit requested (linux)
    if (buttons & Input::Button::QUIT) {
        p_platform->getInput()->setRepeatDelay(0);
        return false;
    }

    if (buttons & Input::Button::UP) {
        int index = m_file_index + m_highlight_index;
        int middle = m_max_lines / 2;
        if (m_highlight_index <= middle && index - middle > 0) {
            m_file_index--;
        } else {
            m_highlight_index--;
        }
        if (m_highlight_index < 0) {
            m_highlight_index = m_files.count < m_max_lines - 1 ? m_files.count - 1 : m_max_lines - 1;
            m_file_index = m_files.count - 1 - m_highlight_index;
        }
        //flip();
    } else if (buttons & Input::Button::DOWN) {
        int index = m_file_index + m_highlight_index;
        int middle = m_max_lines / 2;
        if (m_highlight_index >= middle && index + middle < m_files.count) {
            m_file_index++;
        } else {
            m_highlight_index++;
        }
        if (m_highlight_index >= m_max_lines || m_file_index + m_highlight_index >= m_files.count) {
            m_file_index = 0;
            m_highlight_index = 0;
        }
        //flip();
    } else if (buttons & Input::Button::LEFT) {
        if (m_file_index > 0) {
            int index = m_file_index - m_max_lines;
            if (index < 0) index = 0;
            setSelection(index);
        }
    } else if (buttons & Input::Button::RIGHT) {
        if (m_file_index < m_files.count) {
            int index = m_file_index + m_max_lines;
            if (index > m_files.count - 1) index = m_files.count - 1;
            setSelection(index);
        }
    } else if (buttons & Input::Button::B1) {
        m_rom = Io::getRomPath() + "/" + m_files.get(m_file_index + m_highlight_index);
        auto file = p_platform->getIo()->read(m_rom, Io::Target::FlashRomData);
        if (!file.data) {
            printf("InfoNES::loadRom: failed to load rom (%s)\r\n", m_rom.c_str());
        } else {
            return false;
        }
    }

    flip();

    return true;
}

void Ui::flip() {
    auto display = p_platform->getDisplay();

    //display->clear(Color::GrayDark);
    display->clear(Color::Black);
    display->setTextColor(Color::Yellow);

    for (int i = 0; i < m_max_lines; i++) {
        if (m_file_index + i >= m_files.count) break;

        auto y = (int16_t) ((m_line_height * i) + 6);
        if (i == m_highlight_index) {
            display->fillRect(1, (int16_t) (y - 4), (int16_t) (display->getSize().x - 3),
                              (int16_t) (m_line_height + 2), Color::Green);
        }

        display->drawText(6, y + 13, m_files.get(i + m_file_index));
    }

    display->drawRect(0, 0, display->getSize().x, display->getSize().y, Color::Red);

    display->flip();
}

Platform *Ui::getPlatform() {
    return s_platform;
}

Display *Ui::getDisplay() {
    return s_platform->getDisplay();
}
