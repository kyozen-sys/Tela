#pragma once

#include <memory>
#include <string_view>
#include <functional>

namespace tela
{

class Window
{
public:
    struct Handle {
        void* display;
        void* surface;
    };

    [[nodiscard]] static std::unique_ptr<Window> create(int width, int height, std::string_view title);

    Window() = default;

    Window(const Window&) = delete;

    virtual ~Window() = default;

    Window& operator=(const Window&) = delete;

    [[nodiscard]] virtual bool is_open() const = 0;

    [[nodiscard]] virtual int width() const = 0;

    [[nodiscard]] virtual int height() const = 0;

    [[nodiscard]] virtual Handle native_handle() const = 0;

    virtual void set_resize_handler(std::function<void(int, int)> handler) = 0;

    virtual void poll_events() = 0;
};

}