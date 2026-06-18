#include "window.hpp"

#include <cstring>
#include <stdexcept>

std::unique_ptr<tela::Window>
tela::Window::create(int width, int height, std::string_view title) {
    return std::make_unique<tela::platform::wayland::WaylandWindow>(width, height, title);
}

namespace
{

static void on_xdg_wm_base_ping(void*, xdg_wm_base* wm, uint32_t serial) {
    xdg_wm_base_pong(wm, serial);
}

static constexpr xdg_wm_base_listener wxdg_wm_base_listener = {
    .ping = on_xdg_wm_base_ping
};

static void on_wl_global(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t) {
    auto* impl = static_cast<tela::platform::wayland::WaylandWindowImpl*>(data);

    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        impl->wwl_compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        impl->wxdg_wm_base = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
    
        xdg_wm_base_add_listener(impl->wxdg_wm_base, &wxdg_wm_base_listener, impl);
    }
}

static void on_wl_global_remove(void*, wl_registry*, uint32_t) {}

static constexpr wl_registry_listener wwl_registry_listener = {
    .global = on_wl_global,
    .global_remove = on_wl_global_remove
};

static void on_xdg_surface_configure(void* data, xdg_surface* surface, uint32_t serial) {
    auto* impl = static_cast<tela::platform::wayland::WaylandWindowImpl*>(data);

    xdg_surface_ack_configure(surface, serial);

    impl->configured = true;    
}

static constexpr xdg_surface_listener wxdg_surface_listener = {
    .configure = on_xdg_surface_configure
};

static void on_xdg_toplevel_configure(void*, xdg_toplevel*, int32_t, int32_t, wl_array*) {}

static void on_xdg_toplevel_close(void* data, xdg_toplevel*) {
    auto* impl = static_cast<tela::platform::wayland::WaylandWindowImpl*>(data);

    impl->open = false;
}

static constexpr xdg_toplevel_listener wxdg_toplevel_listener = {
    .configure = on_xdg_toplevel_configure,
    .close = on_xdg_toplevel_close,
    .configure_bounds = nullptr,
    .wm_capabilities = nullptr
};

}

namespace tela::platform::wayland {

WaylandWindow::WaylandWindow(int width, int height, std::string_view title) : impl_(new WaylandWindowImpl{width, height}) {
    impl_->wwl_display = wl_display_connect(nullptr);

    if (!impl_->wwl_display)
        throw std::runtime_error("Failed to connect to Wayland display");

    impl_->wwl_registry = wl_display_get_registry(impl_->wwl_display);

    if (!impl_->wwl_registry)
        throw std::runtime_error("Failed to get Wayland registry");

    wl_registry_add_listener(impl_->wwl_registry, &wwl_registry_listener, impl_);

    wl_display_roundtrip(impl_->wwl_display);

    if (!impl_->wwl_compositor || !impl_->wxdg_wm_base)
        throw std::runtime_error("Failed to bind to required Wayland globals");

    impl_->wwl_surface = wl_compositor_create_surface(impl_->wwl_compositor);

    if (!impl_->wwl_surface)
        throw std::runtime_error("Failed to create Wayland surface");

    impl_->wxdg_surface = xdg_wm_base_get_xdg_surface(impl_->wxdg_wm_base, impl_->wwl_surface);

    if (!impl_->wxdg_surface)
        throw std::runtime_error("Failed to get xdg_surface");

    xdg_surface_add_listener(impl_->wxdg_surface, &wxdg_surface_listener, impl_);

    impl_->wxdg_toplevel = xdg_surface_get_toplevel(impl_->wxdg_surface);

    if (!impl_->wxdg_toplevel)
        throw std::runtime_error("Failed to get xdg_toplevel");
    
    xdg_toplevel_add_listener(impl_->wxdg_toplevel, &wxdg_toplevel_listener, impl_);

    xdg_toplevel_set_title(impl_->wxdg_toplevel, title.data());

    wl_surface_commit(impl_->wwl_surface);

    wl_display_roundtrip(impl_->wwl_display);

    impl_->open = true;
}

WaylandWindow::~WaylandWindow() {
    if (impl_->wxdg_toplevel)  xdg_toplevel_destroy(impl_->wxdg_toplevel);
    
    if (impl_->wxdg_surface) xdg_surface_destroy(impl_->wxdg_surface);
    
    if (impl_->wwl_surface)  wl_surface_destroy(impl_->wwl_surface);
    
    if (impl_->wxdg_wm_base)  xdg_wm_base_destroy(impl_->wxdg_wm_base);
    
    if (impl_->wwl_compositor) wl_compositor_destroy(impl_->wwl_compositor);
    
    if (impl_->wwl_registry) wl_registry_destroy(impl_->wwl_registry);

    if (impl_->wwl_display)  wl_display_disconnect(impl_->wwl_display);

    delete impl_;
}

WaylandWindow::Handle WaylandWindow::native_handle() const {
    return Handle{impl_->wwl_display, impl_->wwl_surface};
}

bool WaylandWindow::is_open() const {
    return impl_->open;
}

int WaylandWindow::width() const {
    return impl_->width;
}

int WaylandWindow::height() const {
    return impl_->height;
}

void WaylandWindow::poll_events() {
    wl_display_dispatch_pending(impl_->wwl_display);

    wl_display_flush(impl_->wwl_display);
}

}