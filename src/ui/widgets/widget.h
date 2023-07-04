//
// Created by cpasjuste on 30/03/23.
//

#ifndef U_GAMEPAD_WIDGET_H
#define U_GAMEPAD_WIDGET_H

#include <cstdint>
#include <vector>

namespace mb {
    class Widget {
    public:
        enum class Origin : uint8_t {
            Left, TopLeft, Top, TopRight,
            Right, BottomRight, Bottom, BottomLeft, Center
        };

        enum class Visibility : uint8_t {
            Visible, Hidden
        };

        Widget() = default;

        Widget(const Utility::Vec2i &pos, const Utility::Vec2i &size) {
            m_position = pos;
            m_size = size;
            m_color = Display::Color::Black;
        }

        ~Widget();

        virtual void add(Widget *widget);

        virtual void remove(Widget *widget);

        virtual void setPosition(int16_t x, int16_t y);

        virtual void setPosition(const Utility::Vec2i &pos);

        virtual Utility::Vec2i getPosition();

        virtual void setSize(int16_t x, int16_t y);

        virtual void setSize(const Utility::Vec2i &size);

        virtual Utility::Vec2i getSize() { return m_size; };

        virtual Utility::Vec4i getBounds();

        virtual void setOrigin(const Origin &origin) { m_origin = origin; };

        virtual Origin getOrigin() { return m_origin; };

        virtual void setColor(uint16_t color) { m_color = color; };

        virtual uint16_t getColor() { return m_color; };

        virtual bool isVisible() { return m_visibility == Visibility::Visible; };

        virtual void setVisibility(Visibility visibility) { m_visibility = visibility; };

        virtual void loop(const Utility::Vec2i &pos, const uint16_t &buttons);

    protected:
        Widget *p_parent = nullptr;
        std::vector<Widget *> p_child_list;
        Visibility m_visibility = Visibility::Visible;
        Utility::Vec2i m_position{};
        Utility::Vec2i m_size{};
        Origin m_origin = Origin::TopLeft;
        uint16_t m_color = Display::Color::White;
    };
}

#endif //U_GAMEPAD_WIDGET_H
