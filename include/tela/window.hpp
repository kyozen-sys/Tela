#pragma once

#include <memory>
#include <string_view>

namespace tela
{

class Window
{
public:
    struct Handle {
        void* display;
        void* surface;
    };

    Window() = default;

    Window(const Window&) = delete;

    virtual ~Window() = default;

    Window& operator=(const Window&) = delete;

    [[nodiscard]] static std::unique_ptr<Window>
    create(int width, int height, std::string_view title);

    [[nodiscard]] virtual Handle native_handle() const = 0;
};

}