#include "tela/renderer.hpp"

#include "renderer/opengl/renderer.hpp"

#include <wayland-egl.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

std::unique_ptr<tela::Renderer> tela::Renderer::create(const Window& window) {
    auto handle = window.native_handle();

    auto* wwl_display = static_cast<struct wl_display*>(handle.display);

    auto* wwl_surface = static_cast<struct wl_surface*>(handle.surface);

    EGLDisplay egl_display = eglGetPlatformDisplay(EGL_PLATFORM_WAYLAND_KHR, wwl_display, nullptr);

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

    auto native_window = reinterpret_cast<EGLNativeWindowType>(egl_window.get());

    return std::make_unique<platform::opengl::OpenGLRenderer>(egl_display, native_window, [window = egl_window.release()]{
        wl_egl_window_destroy(window);
    });
}