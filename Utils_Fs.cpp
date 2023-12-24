#include "Utils_Fs.hpp"

bool util::fs::isSubpath(const std::filesystem::path& path, const std::filesystem::path& base)
{
    auto rel = std::filesystem::relative(path, base);
    return !rel.empty() && rel.native()[0] != '.';
}