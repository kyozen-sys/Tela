#pragma once

#include "tela/window.hpp"
#include "tela/texture.hpp"

namespace tela
{

class Renderer
{
public:
    [[nodiscard]] static std::unique_ptr<Renderer> create(Window& window);

    Renderer() = default;

    virtual ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    
    Renderer& operator=(const Renderer&) = delete;

    virtual void clear(float r = 0.f, float g = 0.f, float b = 0.f, float a = 0.f) = 0;

    virtual void present() = 0;

    virtual void draw_texture2d(const Texture2D& texture, float x, float y, float width = 0.f, float height = 0.f) = 0;

    virtual void on_resize(int width, int height) = 0;
};

}