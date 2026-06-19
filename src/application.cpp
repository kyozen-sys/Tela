#include "tela/application.hpp"

#include "tela/input.hpp"

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
    while (impl_->window->is_open()) {
        Input::advance_frame();

        impl_->window->poll_events();

        impl_->scene.tick();

        impl_->scene.draw(*impl_->renderer);

        impl_->renderer->present();
    }
}

}