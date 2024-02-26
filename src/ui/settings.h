//
// Created by cpasjuste on 04/07/23.
//

#ifndef RETROPICO_SETTINGS_H
#define RETROPICO_SETTINGS_H

#include "rectangle.h"
#include "text.h"
#include "bitmap.h"

namespace mb {
    class Settings : public p2d::Rectangle {
    public:
        Settings(const p2d::Utility::Vec2i &pos, const p2d::Utility::Vec2i &size);
    };
}

#endif //RETROPICO_SETTINGS_H
