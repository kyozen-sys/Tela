#include <iostream>

#include <tela/application.hpp>

int main() {
    tela::Application app{800, 600, "Pong"};

    app.run();

    return 0;
}