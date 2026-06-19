#include "tela/application.hpp"

#include <chrono>
#include <algorithm>

#include <iostream>

namespace tela
{

struct Application::Impl {
    int width;
    int height;

    std::string title;

    std::unique_ptr<Window> window = nullptr;
    std::unique_ptr<Renderer> renderer = nullptr;

    SceneNode scene = SceneNode();
};

Application::Application(int width, int height, std::string_view title) : impl_(std::make_unique<Impl>(Impl{width, height, std::string(title)})) {
    impl_->window = Window::create(width, height, title);

    impl_->renderer = Renderer::create(*impl_->window);

    impl_->scene.set_size(width, height);
}

Application::~Application() {}

SceneNode& Application::scene() {
    return impl_->scene;
}

Renderer& Application::renderer() {
    return *impl_->renderer;
}

void Application::run() {
    using clock = std::chrono::steady_clock;

    auto last = clock::now();

    while (impl_->window->is_open()) {
        impl_->window->poll_events();

        auto now = clock::now();

        float delta = std::chrono::duration<float>(now - last).count();

        last = now;

        impl_->scene.tick(delta);

        impl_->scene.draw(*impl_->renderer);

        impl_->renderer->present();
    }
}

}