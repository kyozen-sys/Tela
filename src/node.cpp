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

struct Scene::Impl {
    float clear_r = 1.0f;
    float clear_g = 1.0f;
    float clear_b = 1.0f;
    float clear_a = 1.0f;

    std::chrono::steady_clock::time_point last_tick = std::chrono::steady_clock::now();
};

Scene::Scene() : impl_(std::make_unique<Impl>()) {}

Scene::~Scene() = default;
 
void Scene::draw(Renderer& renderer) {
    renderer.clear(impl_->clear_r, impl_->clear_g, impl_->clear_b, impl_->clear_a);

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
    impl_->clear_r = r; impl_->clear_g = g; impl_->clear_b = b; impl_->clear_a = a;
}

void Scene::tick() {
    auto now = std::chrono::steady_clock::now();

    float delta = std::chrono::duration<float>(now - impl_->last_tick).count();

    impl_->last_tick = now;

    Node::tick(delta);
}

struct Node2D::Impl {
    Node2D::Size size;

    Node2D::Position position;
};

Node2D::Node2D() : impl_(std::make_unique<Impl>()) {}

Node2D::~Node2D() = default;

Node2D::Size Node2D::size() const {
    auto [natural_width, natural_height] = natural_size();

    const int width = impl_->size.width > 0 ? impl_->size.width : natural_width;

    const int height = impl_->size.height > 0 ? impl_->size.height : natural_height;

    return { width, height };
}

void Node2D::set_size(int width, int height) {
    impl_->size.width = width; impl_->size.height = height;
}

Node2D::Position Node2D::position() const {
    return impl_->position;
}

void Node2D::set_position(float x, float y) {
    impl_->position.x = x; impl_->position.y = y;
}

void Node2D::set_position(Node2D::Position position) {
    impl_->position = position;
}

struct Sprite2D::Impl {
    std::shared_ptr<Texture2D> texture2d;
};

Sprite2D::Sprite2D(std::string_view path) : Node2D(), impl_(std::make_unique<Impl>()) {
    impl_->texture2d = Texture2D::create(path);
}

Sprite2D::~Sprite2D() = default;

void Sprite2D::draw(Renderer& renderer) {
    auto [width, height] = size();

    auto [x, y] = position();

    renderer.draw_texture2d(*impl_->texture2d, x, y, static_cast<float>(width), static_cast<float>(height));
}

std::pair<int, int> Sprite2D::natural_size() const {
    return { impl_->texture2d->width(), impl_->texture2d->height() };
}

}