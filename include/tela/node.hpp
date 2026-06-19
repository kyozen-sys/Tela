#pragma once

#include "tela/renderer.hpp"

#include <span>
#include <vector>

namespace tela
{

class Node
{
friend class Application;

friend class SceneNode;

public:
    virtual ~Node() = default;

    virtual void on_ready() {};

    virtual void process(float) {};

    virtual void draw(Renderer&) {};

    void set_layer(int layer);

    int layer() const;

    void add_child(Node& child);

    void remove_child(Node& child);

    std::span<Node* const> children() const;
private:
    int layer_ = 0;

    std::vector<Node*> children_;

    void tick(float delta);
    
    void collect(std::vector<Node*>& out);
};

class SceneNode : public Node 
{
public:
    void draw(Renderer& renderer) override;
    
    int width() const;

    int height() const;

    void set_clear_color(float r, float g, float b, float a);

    void set_size(int width, int height);

private:
    float clear_r_ = 0.0f;
    float clear_g_ = 0.0f;
    float clear_b_ = 0.0f;
    float clear_a_ = 1.0f;

    int width_ = 0;
    int height_ = 0;
};

}