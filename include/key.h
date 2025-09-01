#pragma once

#include <format>
#include <string_view>

#include "log.h"

using namespace std::literals;

namespace game
{
    enum class Key
    {
        TAB = 0x9,
        RETURN = 0xd,

#ifdef WIN32
        LSHIFT = 0x10,
        LCTRL = 0x11,
#else
        LSHIFT = 0x154,
        LCTRL = 0x155,
#endif

        ESC = 0x1b,

        SPACE = 0x20,
        PG_UP = 0x21,
        PG_DOWN = 0x22,
        END = 0x23,
        HOME = 0x24,
#ifdef WIN32
        LEFT_ARROW = 0x25,
        UP_ARROW = 0x26,
        RIGHT_ARROW = 0x27,
        DOWN_ARROW = 0x28,
#endif

        INSERT = 0x2d,
        DELETE = 0x2e,

        KEY_0 = 0x30,
        KEY_1 = 0x31,
        KEY_2 = 0x32,
        KEY_3 = 0x33,
        KEY_4 = 0x34,
        KEY_5 = 0x35,
        KEY_6 = 0x36,
        KEY_7 = 0x37,
        KEY_8 = 0x38,
        KEY_9 = 0x39,

        A = 0x41,
        B = 0x42,
        C = 0x43,
        D = 0x44,
        E = 0x45,
        F = 0x46,
        G = 0x47,
        H = 0x48,
        I = 0x49,
        J = 0x4a,
        K = 0x4b,
        L = 0x4c,
        M = 0x4d,
        N = 0x4e,
        O = 0x4f,
        P = 0x50,
        Q = 0x51,
        R = 0x52,
        S = 0x53,
        T = 0x54,
        U = 0x55,
        V = 0x56,
        W = 0x57,
        X = 0x58,
        Y = 0x59,
        Z = 0x5a,

        NUMPAD_0 = 0x60,
        NUMPAD_1 = 0x61,
        NUMPAD_2 = 0x62,
        NUMPAD_3 = 0x63,
        NUMPAD_4 = 0x64,
        NUMPAD_5 = 0x65,
        NUMPAD_6 = 0x66,
        NUMPAD_7 = 0x67,
        NUMPAD_8 = 0x68,
        NUMPAD_9 = 0x69,

#ifndef WIN32
        RIGHT_ARROW = 0x106,
        LEFT_ARROW = 0x107,
        DOWN_ARROW = 0x108,
        UP_ARROW = 0x109,
#endif

    };

    enum class KeyState
    {
        UP,
        DOWN
    };
}

template <>
struct std::formatter<game::Key>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::Key &obj, std::format_context &ctx) const
    {
        auto s = "?"sv;

        switch (obj)
        {
            using enum game::Key;
        case TAB:
            s = "TAB"sv;
            break;
        case RETURN:
            s = "RETURN"sv;
            break;
        case LSHIFT:
            s = "LSHIFT"sv;
            break;
        case LCTRL:
            s = "LCTRL"sv;
            break;
        case ESC:
            s = "ESC"sv;
            break;
        case SPACE:
            s = "SPACE"sv;
            break;
        case PG_UP:
            s = "PG_UP"sv;
            break;
        case PG_DOWN:
            s = "PG_DOWN"sv;
            break;
        case END:
            s = "END"sv;
            break;
        case HOME:
            s = "HOME"sv;
            break;
        case LEFT_ARROW:
            s = "LEFT_ARROW"sv;
            break;
        case UP_ARROW:
            s = "UP_ARROW"sv;
            break;
        case RIGHT_ARROW:
            s = "RIGHT_ARROW"sv;
            break;
        case DOWN_ARROW:
            s = "DOWN_ARROW"sv;
            break;
        case INSERT:
            s = "INSERT"sv;
            break;
        case DELETE:
            s = "DELETE"sv;
            break;

        case KEY_0:
            s = "0"sv;
            break;
        case KEY_1:
            s = "1"sv;
            break;
        case KEY_2:
            s = "2"sv;
            break;
        case KEY_3:
            s = "3"sv;
            break;
        case KEY_4:
            s = "4"sv;
            break;
        case KEY_5:
            s = "5"sv;
            break;
        case KEY_6:
            s = "6"sv;
            break;
        case KEY_7:
            s = "7"sv;
            break;
        case KEY_8:
            s = "8"sv;
            break;
        case KEY_9:
            s = "9"sv;
            break;

        case NUMPAD_0:
            s = "0"sv;
            break;
        case NUMPAD_1:
            s = "1"sv;
            break;
        case NUMPAD_2:
            s = "2"sv;
            break;
        case NUMPAD_3:
            s = "3"sv;
            break;
        case NUMPAD_4:
            s = "4"sv;
            break;
        case NUMPAD_5:
            s = "5"sv;
            break;
        case NUMPAD_6:
            s = "6"sv;
            break;
        case NUMPAD_7:
            s = "7"sv;
            break;
        case NUMPAD_8:
            s = "8"sv;
            break;
        case NUMPAD_9:
            s = "9"sv;
            break;

        case A:
            s = "A"sv;
            break;
        case B:
            s = "B"sv;
            break;
        case C:
            s = "C"sv;
            break;
        case D:
            s = "D"sv;
            break;
        case E:
            s = "E"sv;
            break;
        case F:
            s = "F"sv;
            break;
        case G:
            s = "G"sv;
            break;
        case H:
            s = "H"sv;
            break;
        case I:
            s = "I"sv;
            break;
        case J:
            s = "J"sv;
            break;
        case K:
            s = "K"sv;
            break;
        case L:
            s = "L"sv;
            break;
        case M:
            s = "M"sv;
            break;
        case N:
            s = "N"sv;
            break;
        case O:
            s = "O"sv;
            break;
        case P:
            s = "P"sv;
            break;
        case Q:
            s = "Q"sv;
            break;
        case R:
            s = "R"sv;
            break;
        case S:
            s = "S"sv;
            break;
        case T:
            s = "T"sv;
            break;
        case U:
            s = "U"sv;
            break;
        case V:
            s = "V"sv;
            break;
        case W:
            s = "W"sv;
            break;
        case X:
            s = "X"sv;
            break;
        case Y:
            s = "Y"sv;
            break;
        case Z:
            s = "Z"sv;
            break;
        default:
            game::log::debug("key pressed raw: {}", static_cast<int>(obj));
            break;
        }

        return std::format_to(ctx.out(), "{}", s);
    }
};

template <>
struct std::formatter<game::KeyState>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::KeyState &obj, std::format_context &ctx) const
    {
        auto s = "?"sv;

        switch (obj)
        {
            using enum game::KeyState;
        case UP:
            s = "UP"sv;
            break;
        case DOWN:
            s = "DOWN"sv;
            break;
        }

        return std::format_to(ctx.out(), "{}", s);
    }
};
