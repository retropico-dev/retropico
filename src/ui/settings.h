//
// Created by cpasjuste on 04/07/23.
//

#ifndef MICROBOY_SETTINGS_H
#define MICROBOY_SETTINGS_H

#include "rectangle.h"
#include "text.h"
#include "bitmap.h"

namespace mb {
    class Settings : public p2d::Rectangle {
    public:
        Settings(const p2d::Utility::Vec2i &pos, const p2d::Utility::Vec2i &size);
    };
}

#endif //MICROBOY_SETTINGS_H
