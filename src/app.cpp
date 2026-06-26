#include "tela/app.hpp"

#include "tela/input.hpp"

namespace tela
{

struct App::Impl {
    std::unique_ptr<Window> window = nullptr;
    std::unique_ptr<Renderer> renderer = nullptr;
};

App::App(Config config) : impl_(std::make_unique<Impl>()) {
    impl_->window = Window::create(config.width, config.height, config.title, config.resizable);

    impl_->renderer = Renderer::create(*impl_->window);
}

App::~App() = default;

void App::run_scene(Scene& scene) {
    while (impl_->window->is_open()) {
        Input::advance_frame();

        impl_->window->poll_events();

        scene.tick();

        scene.draw(*impl_->renderer);

        impl_->renderer->present();
    }
}

}