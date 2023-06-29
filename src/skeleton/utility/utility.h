//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_UTILITY_H
#define MICROBOY_UTILITY_H

#include <cstdint>
#include <string>
#include <malloc.h>

extern "C" char __StackLimit;
extern "C" char __bss_end__;

namespace mb {
    class Utility {
    public:
        struct Vec2i {
            int16_t x;
            int16_t y;

            Vec2i() : x(0), y(0) {}

            Vec2i(const int16_t x, const int16_t y) : x(x), y(y) {}

            bool operator==(const Vec2i &rhs) const {
                return rhs.x == x && rhs.y == y;
            }

            bool operator!=(const Vec2i &rhs) const {
                return rhs.x != x || rhs.y != y;
            }

            Vec2i operator+(const Vec2i &rhs) const {
                return {(int16_t) (x + rhs.x), (int16_t) (y + rhs.y)};
            }

            Vec2i operator-(const Vec2i &rhs) const {
                return {(int16_t) (x - rhs.x), (int16_t) (y - rhs.y)};
            }

            Vec2i operator*(const int16_t i) const {
                return {(int16_t) (x * i), (int16_t) (y * i)};
            }
        };

        struct Vec4i {
            int16_t x;
            int16_t y;
            int16_t w;
            int16_t h;

            Vec4i() : x(0), y(0), w(0), h(0) {}

            Vec4i(const int16_t x, const int16_t y, const int16_t w, const int16_t h)
                    : x(x), y(y), w(w), h(h) {}

            Vec4i operator+(const Vec4i &rhs) const {
                return {(int16_t) (x + rhs.x), (int16_t) (y + rhs.y),
                        (int16_t) (w + rhs.w), (int16_t) (h + rhs.h)};
            }

            Vec4i operator-(const Vec4i &rhs) const {
                return {(int16_t) (x - rhs.x), (int16_t) (y - rhs.y),
                        (int16_t) (w - rhs.w), (int16_t) (h - rhs.h)};
            }

            Vec4i operator*(const int16_t i) const {
                return {(int16_t) (x * i), (int16_t) (y * i),
                        (int16_t) (w * i), (int16_t) (h * i)};
            }
        };

        static std::string removeExt(const std::string &str);

        static std::string baseName(const std::string &path);

        static inline int getTotalHeap() {
#ifdef LINUX
            return 0;
#else
            return &__StackLimit - &__bss_end__;
#endif
        }

        static inline int getUsedHeap() {
#ifdef LINUX
            return 0;
#else
            struct mallinfo m = mallinfo();
            return m.uordblks;
#endif
        }

        static inline int getFreeHeap() {
            return getTotalHeap() - getUsedHeap();
        }
    };
}

#endif //MICROBOY_UTILITY_H
