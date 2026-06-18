#pragma once

#include "tela/window.hpp"

namespace tela::platform::wayland
{

class WaylandWindow final : public Window
{
public:
    WaylandWindow(int width, int height, std::string_view title);
    ~WaylandWindow() override;

    [[nodiscard]] Handle native_handle() const override;

private:
    struct Impl;
    Impl* impl_;
};

}