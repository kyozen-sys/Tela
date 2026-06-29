#pragma once

#include <filesystem>

#include <string_view>

namespace tela
{

class ResourcePath 
{
friend class Resources;

public:
    const std::filesystem::path& get() const { return resolved_; }

    const char* c_str() const { return resolved_.c_str(); }

    ResourcePath  operator/(std::string_view relative) const {
        return ResourcePath(resolved_ / relative);
    }

private:
    explicit ResourcePath (std::filesystem::path path) : resolved_(std::move(path)) {}

    std::filesystem::path resolved_;
};

class Resources
{
    public:
        [[nodiscard]] static ResourcePath binary_dir();

        [[nodiscard]] static ResourcePath path(std::string_view path_relative);
};

}