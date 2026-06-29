#pragma once

#include "tela/texture.hpp"

namespace tela::backend::opengl
{

struct OpenGLTexture2DImpl {
    unsigned int id;

    int width;
    int height;
};

class OpenGLTexture2D : public Texture2D
{
public:
    OpenGLTexture2D(unsigned int id, int width, int height);

    ~OpenGLTexture2D() override;

    [[nodiscard]] unsigned int gl_id() const;

    int width() const override;

    int height() const override;

private:
    std::unique_ptr<OpenGLTexture2DImpl> impl_;
};

}