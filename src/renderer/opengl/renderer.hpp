#pragma once

#include "tela/renderer.hpp"

#include <EGL/egl.h>

#include <functional>

namespace tela::platform::opengl
{

struct OpenGLRendererImpl {
    EGLDisplay egl_display = EGL_NO_DISPLAY;
    EGLSurface egl_surface = EGL_NO_SURFACE;
    EGLContext egl_context = EGL_NO_CONTEXT;

    std::function<void()> on_destroy;
};

class OpenGLRenderer : public Renderer
{
public:
    OpenGLRenderer(EGLDisplay display, EGLNativeWindowType native_window, std::function<void()> on_destroy);

    ~OpenGLRenderer() override;

    void clear(float r = 0.f, float g = 0.f, float b = 0.f, float a = 0.f) override;

    void present() override;

private:
    OpenGLRendererImpl* impl_;
};

}