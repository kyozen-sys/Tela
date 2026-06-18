#include "window.hpp"

std::unique_ptr<tela::Window>
tela::Window::create(int width, int height, std::string_view title) {
    return std::make_unique<tela::platform::wayland::WaylandWindow>(width, height, title);
}

namespace tela::platform::wayland {

struct WaylandWindow::Impl {
    int width = 0;
    int height = 0;
};

WaylandWindow::WaylandWindow(int width, int height, std::string_view title) : impl_(new Impl{width, height}) {
    (void)title;
}

WaylandWindow::~WaylandWindow() {
    delete impl_;
}

WaylandWindow::Handle WaylandWindow::native_handle() const {
    return Handle{nullptr, nullptr};
}

}