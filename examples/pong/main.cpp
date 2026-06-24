#include <iostream>

#include <tela/app.hpp>

class PongGame : public tela::App
{
public:
    PongGame() : tela::App(800, 600, "Pong") {}
};

int main() {
    PongGame{}.run();
}