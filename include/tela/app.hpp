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

    template<typename T>
    void run() {
        static_assert(std::is_base_of_v<Scene, T>, "T must derive from Scene");

        T scene; run_scene(scene);
    };

private:
    struct Impl;

    std::unique_ptr<Impl> impl_;

    void run_scene(Scene& scene);
};

}