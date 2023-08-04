//
// Created by cpasjuste on 14/06/23.
//

#include "platform.h"
#include "ui.h"
#include "filer.h"

using namespace mb;

Filer::Filer(const Utility::Vec2i &pos, const Utility::Vec2i &size) : Widget(pos, size) {
    p_platform = Ui::getPlatform();

    // get file list
    m_files[Core::Type::Nes] = p_platform->getIo()->getDir(Io::getRomPath(Core::Type::Nes));
    m_files[Core::Type::Gb] = p_platform->getIo()->getDir(Io::getRomPath(Core::Type::Gb));
    m_files[Core::Type::Sms] = p_platform->getIo()->getDir(Io::getRomPath(Core::Type::Sms));

    m_line_height = UI_FONT_HEIGHT + 6; // font height + margin
    m_max_lines = (int16_t) (Filer::getSize().y / m_line_height);
    if (m_max_lines * m_line_height < Filer::getSize().y) {
        m_line_height = (int16_t) (Filer::getSize().y / m_max_lines);
    }

    // add highlight
    p_highlight = new Rectangle({0, 0}, {Filer::getSize().x, (int16_t) (m_line_height + 4)}, true, 8);
    p_highlight->setColor(Ui::Color::Red);
    p_highlight->setOutlineColor(Ui::Color::YellowLight);
    p_highlight->setOutlineThickness(1);
    Filer::add(p_highlight);

    // add lines
    for (int i = 0; i < m_max_lines; i++) {
        auto line = new Text(3, (int16_t) (m_line_height * i + 6),
                             (int16_t) (Filer::getSize().x - 13), (int16_t) m_line_height, "");
        line->setColor(Ui::Color::YellowLight);
        p_lines.push_back(line);
        Filer::add(line);
    }

    // set repeat delay for ui
    p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);
}

void Filer::loop(const Utility::Vec2i &pos, const uint16_t &buttons) {
    if (!isVisible()) return;

    if (!Ui::getInstance()->getMenu()->isVisible()) {
        if (buttons & Input::Button::UP) {
            int index = m_file_index + m_highlight_index;
            int middle = m_max_lines / 2;
            if (m_highlight_index <= middle && index - middle > 0) {
                m_file_index--;
            } else {
                m_highlight_index--;
            }
            if (m_highlight_index < 0) {
                m_highlight_index = m_files[m_core].count < m_max_lines ? m_files[m_core].count - 1 : m_max_lines - 1;
                m_file_index = m_files[m_core].count - 1 - m_highlight_index;
            }
        } else if (buttons & Input::Button::DOWN) {
            int index = m_file_index + m_highlight_index;
            int middle = m_max_lines / 2;
            if (m_highlight_index >= middle && index + middle < m_files[m_core].count) {
                m_file_index++;
            } else {
                m_highlight_index++;
            }
            if (m_highlight_index >= m_max_lines || m_file_index + m_highlight_index >= m_files[m_core].count) {
                m_file_index = 0;
                m_highlight_index = 0;
            }
        } else if (buttons & Input::Button::LEFT) {
            int index = m_file_index + m_highlight_index - m_max_lines;
            if (index < 0) index = 0;
            setSelection(index);
        } else if (buttons & Input::Button::RIGHT) {
            int index = m_file_index + m_highlight_index + m_max_lines;
            if (index > m_files[m_core].count - 1) index = m_files[m_core].count - 1;
            setSelection(index);
        } else if (buttons & Input::Button::B1) {
            std::string name = m_files[m_core].get(m_file_index + m_highlight_index);
            std::string path = Io::getRomPath(m_core) + "/" + name;
            Ui::getInstance()->getInfoBox()->show("Please Wait... Loading...");
            auto success = p_platform->getIo()->writeRomToFlash(path, name);
            if (success) {
                m_done = true;
                return;
            } else {
                printf("Filer: failed to load rom (%s)\r\n", path.c_str());
                Ui::getInstance()->getInfoBox()->hide();
            }
        }
    }

    refresh();

    // draw child's
    Widget::loop(pos, buttons);
}

void Filer::refresh() {
    // update "lines"
    for (int i = 0; i < m_max_lines; i++) {
        if (m_file_index + i >= m_files[m_core].count) {
            p_lines[i]->setVisibility(Visibility::Hidden);
        } else {
            p_lines[i]->setVisibility(Visibility::Visible);
            p_lines[i]->setString(m_files[m_core].get(i + m_file_index));
            if (i == m_highlight_index) {
                p_lines[i]->setColor(Ui::Color::Yellow);
                p_highlight->setPosition(p_highlight->getPosition().x, (int16_t) (p_lines[i]->getPosition().y - 6));
            } else {
                p_lines[i]->setColor(Ui::Color::YellowLight);
            }
        }
    }
}

void Filer::setCore(const Core::Type &core) {
    m_core = core;
    m_file_index = 0;
    m_highlight_index = 0;
    refresh();
    // TODO: fix loop flip called twice
    Ui::getInstance()->loop(true);
    Ui::getInstance()->loop(true);
}

void Filer::setSelection(int index) {
    if (index < m_max_lines / 2) {
        m_file_index = 0;
        m_highlight_index = 0;
    } else if (index > m_files[m_core].count - m_max_lines / 2) {
        m_highlight_index = m_max_lines - 1;
        m_file_index = m_files[m_core].count - 1 - m_highlight_index;
        if (m_highlight_index >= m_files[m_core].count) {
            m_highlight_index = m_files[m_core].count - 1;
            m_file_index = 0;
        }
    } else {
        m_highlight_index = m_max_lines / 2;
        m_file_index = index - m_highlight_index;
    }
}
