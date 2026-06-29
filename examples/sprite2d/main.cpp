#include <iostream>

#include <tela/app.hpp>
#include <tela/input.hpp>

constexpr const unsigned int WINDOW_WIDTH = 600;
constexpr const unsigned int WINDOW_HEIGHT = 600;

class Player : public tela::Node2D
{
public:
    Player() : tela::Node2D() {
        sprite_.set_size(256, 256);

        sprite_.set_position(-(sprite_.size().width / 2), -(sprite_.size().height / 2));

        set_position(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);

        add_child(sprite_);
    }

private:
    tela::Sprite2D sprite_{tela::Resources::path("assets/art.png")};
};

class ExampleGame : public tela::Scene
{
public:
    ExampleGame() {
        add_child(player);
    }

private:
    Player player;
};

int main() {
    tela::App app({
        .width = WINDOW_WIDTH, .height = WINDOW_HEIGHT,
        .resizable = false
    });

    app.run<ExampleGame>();
}