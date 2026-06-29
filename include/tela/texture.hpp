#pragma once

#include "resources.hpp"

#include <memory>
#include <string_view>

namespace tela
{

class Texture2D
{
public:
    [[nodiscard]] static std::shared_ptr<Texture2D> create(ResourcePath path);

    Texture2D() = default;

    Texture2D(const Texture2D&) = delete;

    virtual ~Texture2D() = default;

    Texture2D& operator=(const Texture2D&) = delete;

    [[nodiscard]] virtual int width() const = 0;

    [[nodiscard]] virtual int height() const = 0;
};

}