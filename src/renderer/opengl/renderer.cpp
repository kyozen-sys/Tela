#include "renderer.hpp"

#include <GL/gl.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace tela::platform::opengl
{

OpenGLRenderer::OpenGLRenderer(EGLDisplay display, EGLNativeWindowType native_window, std::function<void()> on_destroy) : impl_(new OpenGLRendererImpl{
    .egl_display = display,
    .on_destroy = std::move(on_destroy)
}) {
    EGLConfig config;
    EGLint num_configs;
    
    static constexpr EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };

    if (!eglChooseConfig(impl_->egl_display, config_attribs, &config, 1, &num_configs) || num_configs == 0)
        throw std::runtime_error("Failed to choose EGL config");
    
    eglBindAPI(EGL_OPENGL_API);

    static constexpr EGLint context_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 3,
        EGL_NONE
    };

    impl_->egl_context = eglCreateContext(impl_->egl_display, config, EGL_NO_CONTEXT, context_attribs);

    if (impl_->egl_context == EGL_NO_CONTEXT)
        throw std::runtime_error("Failed to create EGL context");

    impl_->egl_surface = eglCreateWindowSurface(impl_->egl_display, config, native_window, nullptr);

    if (impl_->egl_surface == EGL_NO_SURFACE)
        throw std::runtime_error("Failed to create EGL surface");

    if (!eglMakeCurrent(impl_->egl_display, impl_->egl_surface, impl_->egl_surface, impl_->egl_context))
        throw std::runtime_error("Failed to make EGL context current");

    EGLint w = 0, h = 0;

    eglQuerySurface(impl_->egl_display, impl_->egl_surface, EGL_WIDTH, &w);
    eglQuerySurface(impl_->egl_display, impl_->egl_surface, EGL_HEIGHT, &h);
    
    glViewport(0, 0, w, h);
}

OpenGLRenderer::~OpenGLRenderer() {
    eglMakeCurrent(impl_->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    eglDestroySurface(impl_->egl_display, impl_->egl_surface);
    eglDestroyContext(impl_->egl_display, impl_->egl_context);

    eglTerminate(impl_->egl_display);

    impl_->on_destroy();
}

void OpenGLRenderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);

    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderer::present() {
    eglSwapBuffers(impl_->egl_display, impl_->egl_surface);
}

}