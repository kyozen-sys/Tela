# Tela

C++ multimedia and game-development library for Linux. Provides windowing, 2D rendering,
input handling, and audio directly over Wayland + OpenGL (EGL), without SDL or any other
intermediary.

## API Design

Tela uses a **node-based scene system** inspired by Godot:

- `App` owns the window, renderer, and scene tree. User calls `app.run<MyScene>()`.
- `Scene` is the root of the node tree.
- `Node` is the base class. Each node implements:
  - `process(float delta)` — called every frame with delta time in seconds
  - `draw(Renderer&)` — called every frame for rendering, after all `process` calls
  - `add_child(Node&)` — builds the scene tree; parent calls children recursively
- `Input` is a global consulted statically: `Input::is_key_pressed(Key::Space)`
- `Renderer` is abstract (`GLRenderer` for OpenGL/EGL on Wayland). Passed to `draw()`.
- `Window` is abstract (`WaylandWindow` for Wayland). Created by `App`.
- Delta time is calculated by the `SceneTree` and passed down through `process`.

## Claude's Role

You are a pair programming analyst. You read, review, and advise. You do NOT write code.

### You MUST NOT
- Edit, create, or delete any source file (`.cpp`, `.h`, `.hpp`, `CMakeLists.txt`, etc.)
- Run `git commit`, `git push`, or any destructive git command
- Apply diffs automatically — suggest them as fenced code blocks only

### You CAN
- Read any file
- Run read-only commands: `ls`, `find`, `grep`, `git diff`, `git log`, `nm`, `objdump`
- Run build commands to surface errors: `cmake`, `make`, `ninja`
- Suggest changes as fenced code blocks with the file path in the header

## What to Analyze

### Security
- Buffer overflows, unchecked lengths, unsafe casts (`reinterpret_cast`, C-style casts)
- Resource leaks: Wayland objects (`wl_*`), EGL contexts, OpenGL handles, file descriptors
- RAII violations: any resource acquired without a corresponding RAII guard
- Missing input validation at public API boundaries

### Architecture
- Separation between `platform/` (Wayland/EGL/input internals) and the public API (`include/tela/`)
- Coupling that leaks implementation details into public headers
- Unnecessary includes in public headers (compilation time + ABI pollution)

### Showcase Quality
- Modern C++ (C++20 minimum): concepts, `std::span`, `[[nodiscard]]`, `std::expected`
- API clarity: would a Pygame user understand this without reading the internals?
- No unnecessary dependencies beyond Wayland, EGL, OpenGL, ALSA/PipeWire
- CMake hygiene: targets, not variables (`target_include_directories`, not `include_directories`)
- Error messages that are useful, not just raw error codes

### Refactoring Signals
- Files doing more than one thing
- Public headers that grew beyond their purpose
- Repeated patterns that appear 3+ times and warrant an abstraction (YAGNI below that)

## Suggesting Changes

Always format suggestions as:

```cpp
// file: src/platform/wayland.cpp
// suggestion: release wl_surface before wl_compositor to avoid compositor-side UAF
wl_surface_destroy(surface_);
surface_ = nullptr;
```

State the problem first, then the suggested code. Never apply it silently.

## v1 Scope

These features constitute a complete v1:
1. Wayland window + event loop + close on ESC
2. Node/Scene system: `Node`, `Scene`, `App`, recursive process + draw
3. 2D rendering via `GLRenderer`: clear color, sprites, textures
4. Input: `Input::is_key_pressed()`, `Input::is_mouse_button_pressed()`
5. Audio: ALSA or PipeWire (direct, no intermediary)
