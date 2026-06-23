#pragma once

#include "tela/window.hpp"

#include "xdg-shell-client-protocol.h"

#include <libdecor-0/libdecor.h>

#include <wayland-client.h>

namespace tela::platform::wayland
{

extern const wl_seat_listener wwl_seat_listener;

struct WaylandWindowImpl {
    int width = 0;
    int height = 0;

    std::string title;

    bool configured = false;
    bool open = false;

    std::function<void(int, int)> on_resize = nullptr;

    wl_display* wwl_display = nullptr;
    wl_registry* wwl_registry = nullptr;
    wl_compositor* wwl_compositor = nullptr;
    wl_surface* wwl_surface = nullptr;
    wl_seat* wwl_seat = nullptr;
    wl_keyboard* wwl_keyboard = nullptr;
    wl_pointer* wwl_pointer = nullptr;

    xdg_wm_base* wxdg_wm_base = nullptr;

    libdecor* ldecor = nullptr;
    libdecor_frame* ldecor_frame = nullptr;
};

class WaylandWindow final : public Window
{
public:
    WaylandWindow(int width, int height, std::string_view title);
    
    ~WaylandWindow() override;

    [[nodiscard]] bool is_open() const override;

    [[nodiscard]] int width() const override;

    [[nodiscard]] int height() const override;

    [[nodiscard]] Handle native_handle() const override;

    void set_resize_handler(std::function<void(int, int)> handler) override;

    void poll_events() override;

private:
    std::unique_ptr<WaylandWindowImpl> impl_;
};

}