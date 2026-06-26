#pragma once

#include "tela/renderer.hpp"

#include "texture.hpp"

#include <epoxy/egl.h>

#include <functional>

namespace tela::platform::opengl
{

struct OpenGLTexture2DPipeline {
    GLuint shader = 0;
    GLuint vao    = 0;
    GLuint vbo    = 0;

    GLint u_projection = -1;
    GLint u_model      = -1;

    static constexpr float geometry[] = {
        0.f, 0.f,  0.f, 0.f,
        1.f, 0.f,  1.f, 0.f,
        1.f, 1.f,  1.f, 1.f,
        0.f, 0.f,  0.f, 0.f,
        1.f, 1.f,  1.f, 1.f,
        0.f, 1.f,  0.f, 1.f,
    };

    void init();
    void destroy();

    void draw(const OpenGLTexture2D& texture, float x, float y, float width, float height, int viewport_width, int viewport_height);
};

struct OpenGLRendererImpl {
    EGLDisplay egl_display = EGL_NO_DISPLAY;
    EGLSurface egl_surface = EGL_NO_SURFACE;
    EGLContext egl_context = EGL_NO_CONTEXT;

    std::function<void()> on_destroy;

    int viewport_width = 0;
    int viewport_height = 0;

    OpenGLTexture2DPipeline textures2d = OpenGLTexture2DPipeline();
};

class OpenGLRenderer : public Renderer
{
public:
    OpenGLRenderer(EGLDisplay display, EGLNativeWindowType native_window, std::function<void()> on_destroy);

    ~OpenGLRenderer() override;

    void clear(float r = 0.f, float g = 0.f, float b = 0.f, float a = 0.f) override;

    void present() override;

    void draw_texture2d(const Texture2D& texture, float x, float y, float width = 0.f, float height = 0.f) override;

    void on_resize(int width, int height) override;

private:
    std::unique_ptr<OpenGLRendererImpl> impl_;
};

}