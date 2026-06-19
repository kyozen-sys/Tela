#pragma once

#include "tela/window.hpp"

#include "xdg-shell-client-protocol.h"

#include <wayland-client.h>

namespace tela::platform::wayland
{

struct WaylandWindowImpl {
    int width = 0;
    int height = 0;

    std::string title;

    bool configured = false;
    bool open = false;

    wl_display* wwl_display = nullptr;
    wl_registry* wwl_registry = nullptr;
    wl_compositor* wwl_compositor = nullptr;
    wl_surface* wwl_surface = nullptr;

    xdg_wm_base* wxdg_wm_base = nullptr;
    xdg_surface* wxdg_surface = nullptr;
    xdg_toplevel* wxdg_toplevel = nullptr;
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

    void poll_events() override;

private:
    std::unique_ptr<WaylandWindowImpl> impl_;
};

}