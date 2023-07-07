//
// Created by cpasjuste on 04/07/23.
//

#include "platform.h"
#include "menu.h"
#include "ui.h"
#include "bitmap.h"

#include "bitmaps/nes.h"
#include "bitmaps/gameboy.h"
#include "bitmaps/settings.h"

using namespace mb;

Menu::MenuLine::MenuLine(const Utility::Vec4i &bounds, const Bitmap::Image *image, const std::string &text)
        : Rectangle(bounds, Ui::Color::Transparent, 8) {
    // outline
    Menu::MenuLine::setOutlineColor(Ui::Color::Transparent);
    Menu::MenuLine::setOutlineThickness(2);

    // add icon
    p_bitmap = new Bitmap({10, (int16_t) (getSize().y / 2)}, image);
    p_bitmap->setOrigin(Origin::Left);
    Menu::MenuLine::add(p_bitmap);

    // add text
    p_text = new Text((int16_t) (p_bitmap->getSize().x + 18), (int16_t) (getSize().y / 2 - 8), 150, 64, text);
    p_text->setColor(Ui::Color::YellowLight);
    Menu::MenuLine::add(p_text);
}

Menu::Menu(const Utility::Vec2i &pos, const Utility::Vec2i &size)
        : Rectangle(pos, size, Ui::Color::Gray, 8) {
    Menu::setOutlineColor(Ui::Color::Red);
    Menu::setOutlineThickness(2);

    int16_t h = (int16_t) (getSize().y / 3);
    auto line = new MenuLine({0, 0, getSize().x, h}, &nes_img, "NES");
    line->setOutlineColor(Ui::Color::Red);
    line->p_text->setColor(Ui::Color::Yellow);
    add(line);
    m_lines.emplace_back(line);

    line = new MenuLine({0, h, getSize().x, h}, &gameboy_img, "GAMEBOY");
    add(line);
    m_lines.emplace_back(line);

    line = new MenuLine({0, (int16_t) (h * 2), getSize().x, h}, &settings_img, "SETTINGS");
    add(line);
    m_lines.emplace_back(line);
}

void Menu::loop(const Utility::Vec2i &pos, const uint16_t &buttons) {
    if (!isVisible()) return;

    if (buttons & Input::Button::UP) {
        m_line_index--;
        if (m_line_index < 0) {
            m_line_index = (int8_t) (m_lines.size() - 1);
        }
        refresh();
    } else if (buttons & Input::Button::DOWN) {
        m_line_index++;
        if (m_line_index >= m_lines.size()) {
            m_line_index = 0;
        }
        refresh();
    } else if (buttons & Input::Button::B2) {
        setVisibility(Visibility::Hidden);
    }

    Rectangle::loop(pos, buttons);
}

void Menu::refresh() {
    if (m_line_index == 0) {
        Ui::getInstance()->getFiler()->setCore(Core::Nes);
    } else if (m_line_index == 1) {
        Ui::getInstance()->getFiler()->setCore(Core::Gb);
    } else {
        // TODO: settings
    }

    for (int i = 0; i < m_lines.size(); i++) {
        if (i == m_line_index) {
            m_lines.at(i)->setOutlineColor(Ui::Color::Red);
            m_lines.at(i)->p_text->setColor(Ui::Color::Yellow);
        } else {
            m_lines.at(i)->setOutlineColor(Ui::Color::Transparent);
            m_lines.at(i)->p_text->setColor(Ui::Color::YellowLight);
        }
    }
}
