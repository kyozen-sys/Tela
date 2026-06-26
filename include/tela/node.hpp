#pragma once

#include "tela/renderer.hpp"

#include <span>
#include <chrono>
#include <vector>

namespace tela
{

class Node
{
friend class Scene;

public:
    virtual ~Node() = default;

    virtual void on_ready() {};

    virtual void process(float) {};

    virtual void draw(Renderer&) {};

    void set_layer(int layer);

    int layer() const;

    void add_child(Node& child);

    void remove_child(Node& child);

    Node* parent() const;

    std::span<Node* const> children() const;
private:
    int layer_ = 0;

    Node* parent_ = nullptr;

    std::vector<Node*> children_;

    void tick(float delta);
    
    void collect(std::vector<Node*>& out);
};

class Scene : public Node
{
friend class App;

public:
    Scene();

    ~Scene();

    void draw(Renderer& renderer) override;

    void set_clear_color(float r, float g, float b, float a);

private:
    struct Impl;

    std::unique_ptr<Impl> impl_;

    void tick();
};

class Node2D : public Node
{
public:
    struct Size {
        int width = 0;
        int height = 0;
    };

    struct Position {
        float x = 0;
        float y = 0;
    };

    Node2D();

    ~Node2D();

    Size size() const;

    void set_size(int width, int height);

    void set_size(Size size);

    Position position() const;

    void set_position(float x, float y);

    void set_position(Position position);

protected:
    virtual std::pair<int, int> natural_size() const { return {0, 0}; };

private:
    struct Impl;

    std::unique_ptr<Impl> impl_;
};

class Sprite2D : public Node2D
{
public:
    Sprite2D(std::string_view path);

    ~Sprite2D();

    void draw(Renderer& renderer) override;

protected:
    std::pair<int, int> natural_size() const override;

private:
    struct Impl;

    std::unique_ptr<Impl> impl_;
};

}