#pragma once

#include "tela/renderer.hpp"

#include <EGL/egl.h>

namespace tela::platform::opengl
{

struct OpenGLRendererImpl {
    EGLDisplay egl_display = EGL_NO_DISPLAY;
    EGLSurface egl_surface = EGL_NO_SURFACE;
    EGLContext egl_context = EGL_NO_CONTEXT;
};

class OpenGLRenderer : public Renderer
{
public:
    OpenGLRenderer(const Window& window);

    ~OpenGLRenderer() override;

    void clear(float r = 0.f, float g = 0.f, float b = 0.f, float a = 0.f) override;

    void present() override;

private:
    OpenGLRendererImpl* impl_;
};

}