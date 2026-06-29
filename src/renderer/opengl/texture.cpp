#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <epoxy/gl.h>

std::shared_ptr<tela::Texture2D> tela::Texture2D::create(ResourcePath path) {
    int width, height, channels;

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!data)
        throw std::runtime_error(std::string("Failed to load texture: ") + path.c_str());

    unsigned int id;

    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return std::make_shared<tela::backend::opengl::OpenGLTexture2D>(id, width, height);
}

namespace tela::backend::opengl
{

OpenGLTexture2D::OpenGLTexture2D(unsigned int id, int width, int height) : impl_(std::make_unique<OpenGLTexture2DImpl>(id, width, height)) {}

OpenGLTexture2D::~OpenGLTexture2D() {
    if (impl_->id) glDeleteTextures(1, &impl_->id);
}

unsigned int OpenGLTexture2D::gl_id() const {
    return impl_->id;
}

int OpenGLTexture2D::width() const {
    return impl_->width;
}

int OpenGLTexture2D::height() const {
    return impl_->height;
}

}