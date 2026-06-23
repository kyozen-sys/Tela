#include "window.hpp"

#include <cstring>
#include <stdexcept>

#include <poll.h>

std::unique_ptr<tela::Window>
tela::Window::create(int width, int height, std::string_view title) {
    return std::make_unique<tela::platform::wayland::WaylandWindow>(width, height, title);
}

namespace
{

void on_xdg_wm_base_ping(void*, xdg_wm_base* wm, uint32_t serial) {
    xdg_wm_base_pong(wm, serial);
}

constexpr xdg_wm_base_listener wxdg_wm_base_listener = {
    .ping = on_xdg_wm_base_ping
};

void on_wl_global(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t) {
    auto* impl = static_cast<tela::platform::wayland::WaylandWindowImpl*>(data);

    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        impl->wwl_compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
    }
    
    if (strcmp(interface, wl_seat_interface.name) == 0) {
        impl->wwl_seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, 1));

        wl_seat_add_listener(impl->wwl_seat, &tela::platform::wayland::wwl_seat_listener, impl);
    }
    
    if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        impl->wxdg_wm_base = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
    
        xdg_wm_base_add_listener(impl->wxdg_wm_base, &wxdg_wm_base_listener, impl);
    }
}

void on_wl_global_remove(void*, wl_registry*, uint32_t) {}

constexpr wl_registry_listener wwl_registry_listener = {
    .global = on_wl_global,
    .global_remove = on_wl_global_remove
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

void on_ldecor_error(libdecor*, libdecor_error, const char *message) {
    throw std::runtime_error(std::string("Error captured from libdecor: ") + message);
}

static libdecor_interface ldecor_interface = {
    .error = on_ldecor_error 
};

void on_ldecor_frame_configure(libdecor_frame* frame, libdecor_configuration *configuration, void *data) {
    auto* impl = static_cast<tela::platform::wayland::WaylandWindowImpl*>(data);

    int width = impl->width, height = impl->height;

    libdecor_configuration_get_content_size(configuration, frame, &width, &height);

    impl->width = width; impl->height = height;

    if (impl->on_resize) impl->on_resize(width, height);

    auto* state = libdecor_state_new(width, height);

    libdecor_frame_commit(frame, state, configuration);

    libdecor_state_free(state);
    
    impl->configured = true;
}

void on_ldecor_frame_close(libdecor_frame*, void *data) {
    auto* impl = static_cast<tela::platform::wayland::WaylandWindowImpl*>(data);

    impl->open = false;
}

void on_ldecor_frame_commit(libdecor_frame*, void *data) {
    auto* impl = static_cast<tela::platform::wayland::WaylandWindowImpl*>(data);

    wl_surface_commit(impl->wwl_surface);
}

static libdecor_frame_interface ldecor_frame_interface = {
    .configure = on_ldecor_frame_configure,
    .close = on_ldecor_frame_close,
    .commit = on_ldecor_frame_commit,
};

#pragma GCC diagnostic pop

}

namespace tela::platform::wayland {

WaylandWindow::WaylandWindow(int width, int height, std::string_view title) : impl_(new WaylandWindowImpl{width, height, std::string(title)}) {
    impl_->wwl_display = wl_display_connect(nullptr);

    if (!impl_->wwl_display)
        throw std::runtime_error("Failed to connect to Wayland display");

    impl_->wwl_registry = wl_display_get_registry(impl_->wwl_display);

    if (!impl_->wwl_registry)
        throw std::runtime_error("Failed to get Wayland registry");

    wl_registry_add_listener(impl_->wwl_registry, &wwl_registry_listener, impl_.get());

    wl_display_roundtrip(impl_->wwl_display);

    if (!impl_->wwl_compositor || !impl_->wxdg_wm_base)
        throw std::runtime_error("Failed to bind to required Wayland globals");

    impl_->wwl_surface = wl_compositor_create_surface(impl_->wwl_compositor);

    if (!impl_->wwl_surface)
        throw std::runtime_error("Failed to create Wayland surface");

    impl_->ldecor = libdecor_new(impl_->wwl_display, &ldecor_interface);

    if (!impl_->ldecor)
        throw std::runtime_error("Failed to create ldecor");
    
    impl_->ldecor_frame = libdecor_decorate(impl_->ldecor, impl_->wwl_surface, &ldecor_frame_interface, impl_.get());

    if (!impl_->ldecor_frame)
        throw std::runtime_error("Failed to create ldecor_decorate");

    libdecor_frame_set_title(impl_->ldecor_frame, impl_->title.c_str());

    libdecor_frame_map(impl_->ldecor_frame);

    while (!impl_->configured)
        libdecor_dispatch(impl_->ldecor, -1);

    impl_->open = true;
}

WaylandWindow::~WaylandWindow() {
    if (impl_->ldecor_frame) libdecor_frame_unref(impl_->ldecor_frame);

    if (impl_->ldecor) libdecor_unref(impl_->ldecor);

    if (impl_->wwl_surface) wl_surface_destroy(impl_->wwl_surface);
    
    if (impl_->wxdg_wm_base) xdg_wm_base_destroy(impl_->wxdg_wm_base);
    
    if (impl_->wwl_compositor) wl_compositor_destroy(impl_->wwl_compositor);

    if (impl_->wwl_keyboard) wl_keyboard_destroy(impl_->wwl_keyboard);

    if (impl_->wwl_pointer) wl_pointer_destroy(impl_->wwl_pointer);
    
    if (impl_->wwl_seat) wl_seat_destroy(impl_->wwl_seat);
    
    if (impl_->wwl_registry) wl_registry_destroy(impl_->wwl_registry);

    if (impl_->wwl_display)  wl_display_disconnect(impl_->wwl_display);
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

void WaylandWindow::set_resize_handler(std::function<void(int, int)> handler) {
    impl_->on_resize = std::move(handler);
}

void WaylandWindow::poll_events() {
    wl_display_flush(impl_->wwl_display);

    if (wl_display_prepare_read(impl_->wwl_display) != 0) {
        wl_display_dispatch_pending(impl_->wwl_display); return;
    }

    pollfd pfd = { wl_display_get_fd(impl_->wwl_display), POLLIN, 0 };

    poll(&pfd, 1, 0);

    if (pfd.revents & POLLIN) wl_display_read_events(impl_->wwl_display);
    else {
        wl_display_cancel_read(impl_->wwl_display);
    }

    wl_display_dispatch_pending(impl_->wwl_display);

    if (impl_->ldecor)
        libdecor_dispatch(impl_->ldecor, 0);
}

}