#include "tela/window.hpp"

#include "window.hpp"
#include "input_state.hpp"

#include <wayland-client.h>

namespace
{

void on_wl_keyboard_keymap(void*, wl_keyboard*, uint32_t, int, uint32_t) {}

void on_wl_keyboard_enter(void*, wl_keyboard*, uint32_t, wl_surface*, wl_array*) {}

void on_wl_keyboard_leave(void*, wl_keyboard*, uint32_t, wl_surface*) {}

void on_wl_keyboard_modifiers(void*, wl_keyboard*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) {}

void on_wl_keyboard_repeat_info(void*, wl_keyboard*, int32_t, int32_t) {}

void on_wl_keyboard_key(void*, wl_keyboard*, uint32_t, uint32_t, uint32_t key, uint32_t state) {
    tela::input_on_key(key, state == WL_KEYBOARD_KEY_STATE_PRESSED);
}

constexpr wl_keyboard_listener wwl_keyboard_listener = {
    .keymap = on_wl_keyboard_keymap,
    .enter = on_wl_keyboard_enter,
    .leave = on_wl_keyboard_leave,
    .key = on_wl_keyboard_key,
    .modifiers = on_wl_keyboard_modifiers,
    .repeat_info = on_wl_keyboard_repeat_info
};

void on_wl_pointer_enter(void*, wl_pointer*, uint32_t, wl_surface*, wl_fixed_t, wl_fixed_t) {}

void on_wl_pointer_leave(void*, wl_pointer*, uint32_t, wl_surface*) {}

void on_wl_pointer_axis(void*, wl_pointer*, uint32_t, uint32_t, wl_fixed_t) {}

void on_wl_pointer_frame(void*, wl_pointer*) {}

void on_wl_pointer_axis_source(void*, wl_pointer*, uint32_t) {}

void on_wl_pointer_axis_stop(void*, wl_pointer*, uint32_t, uint32_t) {}

void on_wl_pointer_axis_discrete(void*, wl_pointer*, uint32_t, int32_t) {}

void on_wl_pointer_axis_value120(void*, wl_pointer*, uint32_t, int32_t) {}

void on_wl_pointer_relative_direction(void*, wl_pointer*, uint32_t, uint32_t) {}

void on_wl_pointer_motion(void*, wl_pointer*, uint32_t, wl_fixed_t x, wl_fixed_t y) {
    tela::input_on_pointer_motion(wl_fixed_to_int(x), wl_fixed_to_int(y));
}

void on_wl_pointer_button(void*, wl_pointer*, uint32_t, uint32_t, uint32_t button, uint32_t state) {
    tela::input_on_mouse_button(button, state == WL_POINTER_BUTTON_STATE_PRESSED);
}

constexpr wl_pointer_listener wwl_pointer_listener = {
    .enter = on_wl_pointer_enter,
    .leave = on_wl_pointer_leave,
    .motion = on_wl_pointer_motion,
    .button = on_wl_pointer_button,
    .axis = on_wl_pointer_axis,
    .frame = on_wl_pointer_frame,
    .axis_source = on_wl_pointer_axis_source,
    .axis_stop = on_wl_pointer_axis_stop,
    .axis_discrete = on_wl_pointer_axis_discrete,
    .axis_value120 = on_wl_pointer_axis_value120,
    .axis_relative_direction = on_wl_pointer_relative_direction
};

void on_wl_seat_capabilities(void* data, wl_seat* seat, uint32_t caps) {
    auto* impl = static_cast<tela::platform::wayland::WaylandWindowImpl*>(data);

    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        impl->wwl_keyboard = wl_seat_get_keyboard(seat);

        wl_keyboard_add_listener(impl->wwl_keyboard, &wwl_keyboard_listener, nullptr);
    }

    if (caps & WL_SEAT_CAPABILITY_POINTER) {
        impl->wwl_pointer = wl_seat_get_pointer(seat);

        wl_pointer_add_listener(impl->wwl_pointer, &wwl_pointer_listener, nullptr);
    }
}

void on_wl_seat_name(void*, wl_seat*, const char*) {}

}

namespace tela::platform::wayland {

constexpr wl_seat_listener wwl_seat_listener = {
    .capabilities = on_wl_seat_capabilities,
    .name = on_wl_seat_name
};

}