#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace util::string
{
	std::string_view strip(std::string_view v);
	std::vector<std::string_view> split(std::string_view v, const std::string& delim);
}
