#include "tela/input.hpp"

#include "input_platform.hpp"

#include <bitset>

namespace tela
{

namespace
{
    constexpr unsigned KEY_COUNT  = 768;
    constexpr unsigned BTN_OFFSET = 272;
    constexpr unsigned BTN_COUNT  = 8;

    std::bitset<KEY_COUNT> key_states;
    std::bitset<KEY_COUNT> prev_key_states;

    std::bitset<BTN_COUNT> mouse_states;
    std::bitset<BTN_COUNT> prev_mouse_states;

    int mouse_x = 0, mouse_y = 0;
    int prev_mouse_x = 0, prev_mouse_y = 0;
}

void input_on_key(uint32_t evdev, bool pressed) {
    if (evdev >= KEY_COUNT) return;

    key_states[evdev] = pressed;
}

void input_on_mouse_button(uint32_t btn, bool pressed) {
    if (btn < BTN_OFFSET || btn >= BTN_OFFSET + BTN_COUNT) return;

    mouse_states[btn - BTN_OFFSET] = pressed;
}

void input_on_pointer_motion(int x, int y) {
    mouse_x = x; mouse_y = y;
}

void input_advance_frame() {
    prev_mouse_states = mouse_states;

    prev_key_states = key_states;

    prev_mouse_x = mouse_x; prev_mouse_y = mouse_y;
}

bool Input::is_key_just_pressed(Key key) {
    auto i = static_cast<unsigned>(key);

    return key_states[i] && !prev_key_states[i];
}

bool Input::is_key_just_released(Key key) {
    auto i = static_cast<unsigned>(key);

    return !key_states[i] && prev_key_states[i];
}

bool Input::is_key_pressed(Key key) {
    return key_states[static_cast<unsigned>(key)];
}

bool Input::is_mouse_button_just_pressed(MouseButton btn) {
    auto i = static_cast<unsigned>(btn)  - BTN_OFFSET;

    return mouse_states[i] && !prev_mouse_states[i];
}

bool Input::is_mouse_button_just_released(MouseButton btn) {
    auto i = static_cast<unsigned>(btn) - BTN_OFFSET;

    return !mouse_states[i] && prev_mouse_states[i];
}

bool Input::is_mouse_button_pressed(MouseButton btn) {
    return mouse_states[static_cast<unsigned>(btn) - BTN_OFFSET];
}

std::pair<int, int> Input::mouse_position() {
    return { mouse_x, mouse_y };
}

std::pair<int, int> Input::mouse_delta() {
    return { mouse_x - prev_mouse_x, mouse_y - prev_mouse_y };
}

void Input::advance_frame() {
    input_advance_frame();
}

}