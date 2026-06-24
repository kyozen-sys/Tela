#pragma once

#include <cstdint>

namespace tela
{

void input_on_key(uint32_t evdev_code, bool pressed);

void input_on_mouse_button(uint32_t btn_code, bool pressed);

void input_on_pointer_motion(int x, int y);

void input_advance_frame();

}