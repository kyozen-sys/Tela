#pragma once

#include <utility>

namespace tela {

enum class Key : unsigned int {
    A = 30, B = 48, C = 46, D = 32, E = 18, F = 33, G = 34,
    H = 35, I = 23, J = 36, K = 37, L = 38, M = 50,
    N = 49, O = 24, P = 25, Q = 16, R = 19, S = 31, T = 20,
    U = 22, V = 47, W = 17, X = 45, Y = 21, Z = 44,

    Num1 = 2,  Num2 = 3,  Num3 = 4,  Num4 = 5,  Num5 = 6,
    Num6 = 7,  Num7 = 8,  Num8 = 9,  Num9 = 10, Num0 = 11,

    F1 = 59,  F2 = 60,  F3 = 61,  F4 = 62,  F5 = 63,
    F6 = 64,  F7 = 65,  F8 = 66,  F9 = 67,  F10 = 68,
    F11 = 87, F12 = 88,

    Up = 103, Down = 108, Left = 105, Right = 106,
    Home = 102, End = 107, PageUp = 104, PageDown = 109,
    Insert = 110, Delete = 111,

    Escape    = 1,
    Backspace = 14,
    Tab       = 15,
    Enter     = 28,
    Space     = 57,

    LeftShift  = 42, RightShift  = 54,
    LeftCtrl   = 29, RightCtrl   = 97,
    LeftAlt    = 56, RightAlt    = 100,
};

enum class MouseButton : unsigned int {
    Left   = 272,
    Right  = 273,
    Middle = 274,
    Extra1 = 275,
    Extra2 = 276,
};

class Input {
friend class Application;

public:
    [[nodiscard]] static bool is_key_pressed(Key key);
    [[nodiscard]] static bool is_key_just_pressed(Key key);
    [[nodiscard]] static bool is_key_just_released(Key key);

    [[nodiscard]] static bool is_mouse_button_pressed(MouseButton button);
    [[nodiscard]] static bool is_mouse_button_just_pressed(MouseButton button);
    [[nodiscard]] static bool is_mouse_button_just_released(MouseButton button);

    [[nodiscard]] static std::pair<int, int> mouse_position();
    [[nodiscard]] static std::pair<int, int> mouse_delta();

private:
    static void advance_frame();
};

}
