//
// Created by cpasjuste on 04/07/23.
//

#ifndef MICROBOY_MENU_H
#define MICROBOY_MENU_H

#include "rectangle.h"

namespace mb {
    class Menu : public Rectangle {
    public:
        Menu(const Utility::Vec2i &pos, const Utility::Vec2i &size);
    };
}

#endif //MICROBOY_MENU_H
