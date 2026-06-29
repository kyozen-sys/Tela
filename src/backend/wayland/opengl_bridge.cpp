#include "tela/renderer.hpp"

#include "renderer/opengl/renderer.hpp"

#include <wayland-egl.h>

#include <epoxy/gl.h>
#include <epoxy/egl.h>

std::unique_ptr<tela::Renderer> tela::Renderer::create(Window& window) {
    auto handle = window.native_handle();

    auto* wwl_display = static_cast<struct wl_display*>(handle.display);

    auto* wwl_surface = static_cast<struct wl_surface*>(handle.surface);

    EGLDisplay egl_display = eglGetDisplay(static_cast<EGLNativeDisplayType>(wwl_display));

    if (egl_display == EGL_NO_DISPLAY)
        throw std::runtime_error("Failed to get EGL display");

    if (!eglInitialize(egl_display, nullptr, nullptr))
        throw std::runtime_error("Failed to initialize EGL");

    struct EglWindowDeleter {
        void operator()(wl_egl_window* window) const { wl_egl_window_destroy(window); }
    };

    using EglWindowPtr = std::unique_ptr<wl_egl_window, EglWindowDeleter>;

    EglWindowPtr egl_window(wl_egl_window_create(wwl_surface, window.width(), window.height()));

    if (!egl_window)
        throw std::runtime_error("Failed to create wl_egl_window");

    auto raw_egl_window = egl_window.release();

    auto renderer = std::make_unique<backend::opengl::OpenGLRenderer>(egl_display, reinterpret_cast<EGLNativeWindowType>(raw_egl_window),
        [raw_egl_window] {
            wl_egl_window_destroy(raw_egl_window);
        }
    );

    auto* raw_renderer = renderer.get();

    window.set_resize_handler([raw_egl_window, raw_renderer](int width, int height) {
        wl_egl_window_resize(raw_egl_window, width, height, 0, 0);

        raw_renderer->on_resize(width, height);

        glViewport(0, 0, width, height);
    });

    return renderer;
}