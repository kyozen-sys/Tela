#include "renderer.hpp"

#include <GL/gl.h>

std::unique_ptr<tela::Renderer> tela::Renderer::create(const Window& window) {
    return std::make_unique<platform::opengl::OpenGLRenderer>(window);
}

namespace tela::platform::opengl
{

OpenGLRenderer::OpenGLRenderer(const Window& window) : impl_(new OpenGLRendererImpl{}) {
    auto handle = window.native_handle();

    EGLNativeDisplayType display = reinterpret_cast<EGLNativeDisplayType>(handle.display);

    EGLNativeWindowType egl_win = reinterpret_cast<EGLNativeWindowType>(handle.window);

    impl_->egl_display = eglGetDisplay(display);

    if (impl_->egl_display == EGL_NO_DISPLAY)
        throw std::runtime_error("Failed to get EGL display");

    if (!eglInitialize(impl_->egl_display, nullptr, nullptr))
        throw std::runtime_error("Failed to initialize EGL");
    
    static constexpr EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_CONTEXT_MAJOR_VERSION, 4, EGL_CONTEXT_MINOR_VERSION, 6,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_configs;

    if (!eglChooseConfig(impl_->egl_display, config_attribs, &config, 1, &num_configs) || num_configs == 0)
        throw std::runtime_error("Failed to choose EGL config");
    
    eglBindAPI(EGL_OPENGL_API);

    impl_->egl_context = eglCreateContext(impl_->egl_display, config, EGL_NO_CONTEXT, nullptr);

    if (impl_->egl_context == EGL_NO_CONTEXT)
        throw std::runtime_error("Failed to create EGL context");

    impl_->egl_surface = eglCreateWindowSurface(impl_->egl_display, config, egl_win, nullptr);

    if (impl_->egl_surface == EGL_NO_SURFACE)
        throw std::runtime_error("Failed to create EGL surface");

    if (!eglMakeCurrent(impl_->egl_display, impl_->egl_surface, impl_->egl_surface, impl_->egl_context))
        throw std::runtime_error("Failed to make EGL context current");
}

OpenGLRenderer::~OpenGLRenderer() {
    eglMakeCurrent(impl_->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    eglDestroySurface(impl_->egl_display, impl_->egl_surface);
    eglDestroyContext(impl_->egl_display, impl_->egl_context);

    eglTerminate(impl_->egl_display);

    delete impl_;
}

void OpenGLRenderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);

    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderer::present() {
    eglSwapBuffers(impl_->egl_display, impl_->egl_surface);
}

}