#pragma once

#include "tela/node.hpp"

#include <string_view>

namespace tela
{

class App
{
public:
    App(int width, int height, std::string_view title);

    ~App();

    [[nodiscard]] Scene& scene();

    [[nodiscard]] Renderer& renderer();

    void run();

private:
    struct Impl;

    std::unique_ptr<Impl> impl_;
};

}