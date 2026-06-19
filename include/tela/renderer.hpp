#pragma once

#include "tela/window.hpp"

namespace tela
{

class Renderer
{
public:
    Renderer() = default;

    virtual ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    
    Renderer& operator=(const Renderer&) = delete;

    virtual void clear(float r = 0.f, float g = 0.f, float b = 0.f, float a = 0.f) = 0;

    virtual void present() = 0;

    [[nodiscard]] static std::unique_ptr<Renderer> create(const Window& window);
};

}