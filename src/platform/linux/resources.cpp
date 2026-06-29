#include "tela/resources.hpp"

namespace tela
{

ResourcePath Resources::binary_dir() {
    static ResourcePath dir(
        std::filesystem::canonical("/proc/self/exe").parent_path()
    );

    return dir;
}

ResourcePath Resources::path(std::string_view path) {
    return binary_dir() / path;
}

}