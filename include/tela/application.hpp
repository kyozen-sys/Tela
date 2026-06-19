#pragma once

#include "tela/node.hpp"

#include <string_view>

namespace tela
{

class Application
{
public:
    Application(int width, int height, std::string_view title);

    ~Application();

    [[nodiscard]] SceneNode& scene();

    [[nodiscard]] Renderer& renderer();

    void run();

private:
    struct Impl;

    std::unique_ptr<Impl> impl_;
};

}