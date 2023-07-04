//
// Created by cpasjuste on 04/07/23.
//

#include "platform.h"
#include "menu.h"
#include "ui.h"

using namespace mb;

Menu::Menu(const Utility::Vec2i &pos, const Utility::Vec2i &size) : Rectangle(pos, size) {
    Menu::setColor(Ui::Color::Black);
    Menu::setOutlineColor(Ui::Color::Red);
    Menu::setOutlineThickness(2);
}
