# Tela

C++ multimedia and game-development library. Windowing, 2D rendering, and input via a
pluggable backend system — no SDL or other intermediary.

**Current:** Linux · Wayland · OpenGL/EGL  
**Planned:** Windows, X11, Vulkan

## Dependencies

```sh
# Debian/Ubuntu — Wayland + OpenGL backend
sudo apt install libwayland-dev libdecor-0-dev wayland-protocols \
                 libgl-dev libegl-dev libepoxy-dev
```

## Build

```sh
make          # builds libtela.a
make examples # builds libtela.a + examples
```

CMake directly:

```sh
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build --parallel
```

Backend/renderer selection:

| CMake option        | Values              | Default    |
|---------------------|---------------------|------------|
| `PROJECT_PLATFORM`  | `linux`             | `linux`    |
| `PROJECT_BACKEND`   | `wayland`           | `wayland`  |
| `PROJECT_RENDERER`  | `opengl`            | `opengl`   |

## Usage

```cpp
#include <tela/app.hpp>
#include <tela/input.hpp>

class MyScene : public tela::Scene {
public:
    MyScene() { add_child(player_); }

private:
    Player player_;
};

int main() {
    tela::App app({ .width = 800, .height = 600, .title = "My Game" });
    app.run<MyScene>();
}
```

## Node Types

| Type        | Purpose                                                    |
|-------------|------------------------------------------------------------|
| `Node`      | Base; override `process(float)` and `draw(Renderer&)`      |
| `Scene`     | Root of the tree; owned by `App`                           |
| `Node2D`    | Positioned node with `set_position()` / `size()`          |
| `Sprite2D`  | Renders a texture; constructed from a `ResourcePath`       |

Assets resolve via `tela::Resources::path("assets/foo.png")` relative to the binary.
