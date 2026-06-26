#include "renderer.hpp"

#include <epoxy/gl.h>
#include <epoxy/egl.h>

namespace
{

static constexpr const char* glsl_texture_vertex = R"(
#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

out vec2 v_uv;

uniform mat4 u_projection;
uniform mat4 u_model;

void main() {
    v_uv = uv;

    gl_Position = u_projection * u_model * vec4(pos, 0.0, 1.0);
}
)";

static constexpr const char* glsl_texture_fragment = R"(
#version 330 core

in vec2 v_uv;

uniform sampler2D u_texture;

out vec4 frag_color;

void main() {
    frag_color = texture(u_texture, v_uv);
}
)";

static GLuint compile_program(const char* vertex_src, const char* fragment_src) {
    auto compile = [](GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);

        glShaderSource(shader, 1, &src, nullptr);

        glCompileShader(shader);

        GLint ok = 0;

        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

        if (!ok) {
            char log[512]; glGetShaderInfoLog(shader, sizeof(log), nullptr, log);

            glDeleteShader(shader);

            throw std::runtime_error(std::string("Shader error: ") + log);
        }

        return shader;
    };

    GLuint vertex = compile(GL_VERTEX_SHADER, vertex_src);
    GLuint fragment = compile(GL_FRAGMENT_SHADER, fragment_src);

    GLuint program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

}

namespace tela::platform::opengl
{

void OpenGLTexture2DPipeline::init() {
    shader = compile_program(glsl_texture_vertex, glsl_texture_fragment);

    u_projection = glGetUniformLocation(shader, "u_projection");

    u_model = glGetUniformLocation(shader, "u_model");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void OpenGLTexture2DPipeline::destroy() {
    glDeleteVertexArrays(1, &vao);
    
    glDeleteBuffers(1, &vbo);

    glDeleteProgram(shader);
}

void OpenGLTexture2DPipeline::draw(const OpenGLTexture2D& texture, float x, float y, float width, float height, int viewport_width, int viewport_height) {
    const float projection[16] = {
        2.f/viewport_width, 0.f, 0.f, 0.f,
        0.f, -2.f/viewport_height, 0.f, 0.f,
        0.f, 0.f, -1.f, 0.f,
        -1.f, 1.f, 0.f, 1.f
    };

    const float model[16] = {
        width, 0.f, 0.f, 0.f,
        0.f, height, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        x, y, 0.f, 1.f
    };

    glUseProgram(shader);

    glUniformMatrix4fv(u_projection, 1, GL_FALSE, projection);
    glUniformMatrix4fv(u_model, 1, GL_FALSE, model);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture.gl_id());

    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

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

    EGLint width = 0, height = 0;

    eglQuerySurface(impl_->egl_display, impl_->egl_surface, EGL_WIDTH, &width);
    eglQuerySurface(impl_->egl_display, impl_->egl_surface, EGL_HEIGHT, &height);
    
    glViewport(0, 0, width, height);

    impl_->viewport_width = width; impl_->viewport_height = height;

    impl_->textures2d.init();
}

OpenGLRenderer::~OpenGLRenderer() {
    impl_->textures2d.destroy();

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

void OpenGLRenderer::draw_texture2d(const Texture2D& texture, float x, float y, float width, float height) {
    const auto* gl_texture = dynamic_cast<const OpenGLTexture2D*>(&texture);

    if (!gl_texture) return;

    const float draw_width = (width > 0.f) ? width : static_cast<float>(gl_texture->width());

    const float draw_height = (height > 0.f) ? height : static_cast<float>(gl_texture->height());

    impl_->textures2d.draw(*gl_texture, x, y, draw_width, draw_height, impl_->viewport_width, impl_->viewport_height);
}

void OpenGLRenderer::on_resize(int width, int height) {
    impl_->viewport_width = width; impl_->viewport_height = height;
};

}