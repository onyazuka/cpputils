#pragma once
#include <filesystem>

namespace util::fs {

    bool isSubpath(const std::filesystem::path& path, const std::filesystem::path& base);

}