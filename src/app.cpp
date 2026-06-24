#include "tela/app.hpp"

#include "tela/input.hpp"

namespace tela
{

struct App::Impl {
    int width;
    int height;

    std::string title;

    std::unique_ptr<Window> window = nullptr;
    std::unique_ptr<Renderer> renderer = nullptr;

    Scene scene = Scene();
};

App::App(int width, int height, std::string_view title) : impl_(std::make_unique<Impl>(Impl{width, height, std::string(title)})) {
    impl_->window = Window::create(width, height, title);

    impl_->renderer = Renderer::create(*impl_->window);

    impl_->scene.set_size(width, height);
}

App::~App() {}

Scene& App::scene() {
    return impl_->scene;
}

Renderer& App::renderer() {
    return *impl_->renderer;
}

void App::run() {
    while (impl_->window->is_open()) {
        Input::advance_frame();

        impl_->window->poll_events();

        impl_->scene.tick();

        impl_->scene.draw(*impl_->renderer);

        impl_->renderer->present();
    }
}

}