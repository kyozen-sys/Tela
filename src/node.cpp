#include "tela/node.hpp"

#include <algorithm>

namespace tela
{

void Node::add_child(Node& child) {
    children_.push_back(&child);

    child.on_ready();
}

void Node::remove_child(Node& child) {
    std::erase(children_, &child);
}

std::span<Node* const> Node::children() const {
    return children_;
}

void Node::tick(float delta) {
    process(delta);

    for (auto* child : children_)
        child->tick(delta);
}

void Node::collect(std::vector<Node*>& out) {
    out.push_back(this);

    for (auto* child : children_)
        child->collect(out);
}

void Node::set_layer(int layer) {
    layer_ = layer;
}

int Node::layer() const {
    return layer_;
}

void Scene::draw(Renderer& renderer) {
    renderer.clear(clear_r_, clear_g_, clear_b_, clear_a_);

    std::vector<Node*> drawables;

    for (auto* child : children())
        child->collect(drawables);
    
    std::stable_sort(drawables.begin(), drawables.end(), [](Node* a, Node* b){
        return a->layer() < b->layer();
    });

    for (auto* node : drawables)
        node->draw(renderer);
}

void Scene::set_clear_color(float r, float g, float b, float a) {
    clear_r_ = r; clear_g_ = g; clear_b_ = b; clear_a_ = a;
}

void Scene::set_size(int width, int height) {
    width_ = width;
    height_ = height;
}

void Scene::tick() {
    auto now = std::chrono::steady_clock::now();

    float delta = std::chrono::duration<float>(now - last_tick_).count();

    last_tick_ = now;

    Node::tick(delta);
}

int Scene::width() const {
    return width_;
}

int Scene::height() const {
    return height_;
}

}