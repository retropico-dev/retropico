//
// Created by cpasjuste on 04/07/23.
//

#include "main.h"
#include "menu.h"

using namespace p2d;
using namespace mb;

Menu::MenuLine::MenuLine(const Utility::Vec4i &bounds, const p2d::Io::File &file, const std::string &text)
        : Rectangle(bounds, Display::Color::Transparent) {
    // outline
    MenuLine::setOutlineThickness(1);
    MenuLine::setOutlineColor(Display::Color::Transparent);

    // add icon
    p_bitmap = new Bitmap(file, {2, (int16_t) (getSize().y / 2)});
    p_bitmap->setAlphaEnabled(true);
    p_bitmap->setOrigin(Origin::Left);
    Menu::MenuLine::add(p_bitmap);

    // add text
    p_text = new Text((int16_t) (p_bitmap->getSize().x + 6), (int16_t) (getSize().y / 2) + 2,
                      getSize().x - p_bitmap->getSize().x - 8, (int16_t) (getSize().y / 2), text);
    p_text->setOrigin(Origin::Left);
    p_text->setColor(Ui::Color::Yellow);
    Menu::MenuLine::add(p_text);
}

Menu::Menu(const Utility::Vec2i &pos, const Utility::Vec2i &size) : Rectangle(pos, size, Ui::Color::GrayDark) {
    Menu::setOutlineColor(Ui::Color::Red);
    Menu::setOutlineThickness(1);

    auto h = (int16_t) (getSize().y / 4);
    auto line = new MenuLine({0, 0, (int16_t) (getSize().x - 1), h},
                             Io::File("res:/romfs/nes.bmp"), "NES");
    line->setColor(Ui::Color::Red);
    line->setOutlineColor(Ui::Color::Yellow);
    add(line);
    m_lines.emplace_back(line);

    line = new MenuLine({0, (int16_t) (h + 1), (int16_t) (getSize().x - 1), h},
                        Io::File("res:/romfs/gameboy.bmp"), "GAMEBOY");
    add(line);
    m_lines.emplace_back(line);

    line = new MenuLine({0, (int16_t) (h * 2 - 1), (int16_t) (getSize().x - 1), h},
                        Io::File("res:/romfs/sms.bmp"), "SMS");
    add(line);
    m_lines.emplace_back(line);

    line = new MenuLine({0, (int16_t) (h * 3 - 1), (int16_t) (getSize().x - 1), h},
                        Io::File("res:/romfs/gamegear.bmp"), "GAMEGEAR");
    add(line);
    m_lines.emplace_back(line);

#warning "TODO: settings"
    /*
    line = new MenuLine({0, (int16_t) (h * 3 - 1), (int16_t) (getSize().x - 1), h},
                        Io::File("res:/romfs/settings.bmp"), "SETTINGS");
    add(line);
    m_lines.emplace_back(line);
    */
}

bool Menu::onInput(const uint16_t &buttons) {
    if (!isVisible()) return false;

    if (buttons & Input::Button::UP) {
        m_line_index--;
        if (m_line_index < 0) {
            m_line_index = (int8_t) (m_lines.size() - 1);
        }
        refresh();
        return true;
    } else if (buttons & Input::Button::DOWN) {
        m_line_index++;
        if (m_line_index >= m_lines.size()) {
            m_line_index = 0;
        }
        refresh();
        return true;
    } else if (buttons & Input::Button::B1 || buttons & Input::Button::B2) {
        setVisibility(Visibility::Hidden);
        return true;
    }

    return Rectangle::onInput(buttons);
}

void Menu::refresh() {
    if (m_line_index == 0) {
        Ui::getInstance()->getFiler()->setVisibility(Visibility::Visible);
        Ui::getInstance()->getSettings()->setVisibility(Visibility::Hidden);
        Ui::getInstance()->getFiler()->setCore(Core::Nes);
    } else if (m_line_index == 1) {
        Ui::getInstance()->getFiler()->setVisibility(Visibility::Visible);
        Ui::getInstance()->getSettings()->setVisibility(Visibility::Hidden);
        Ui::getInstance()->getFiler()->setCore(Core::Gb);
    } else if (m_line_index == 2) {
        Ui::getInstance()->getFiler()->setVisibility(Visibility::Visible);
        Ui::getInstance()->getSettings()->setVisibility(Visibility::Hidden);
        Ui::getInstance()->getFiler()->setCore(Core::Sms);
    } else if (m_line_index == 3) {
        Ui::getInstance()->getFiler()->setVisibility(Visibility::Visible);
        Ui::getInstance()->getSettings()->setVisibility(Visibility::Hidden);
        Ui::getInstance()->getFiler()->setCore(Core::Gg);
    }
    /*
    } else {
        Ui::getInstance()->getFiler()->setVisibility(Visibility::Hidden);
        Ui::getInstance()->getSettings()->setVisibility(Visibility::Visible);
    }
    */

    for (int i = 0; i < m_lines.size(); i++) {
        if (i == m_line_index) {
            m_lines.at(i)->setColor(Ui::Color::Red);
            m_lines.at(i)->setOutlineColor(Ui::Color::Yellow);
        } else {
            m_lines.at(i)->setColor(Display::Color::Transparent);
            m_lines.at(i)->setOutlineColor(Display::Color::Transparent);
        }
    }
}
