#include "Utils_String.hpp"

using namespace util::string;

static constexpr char Spaces[] = "\t\n\r ";

std::string_view util::string::strip(std::string_view v) {
	auto p1 = v.find_first_not_of(Spaces);
	auto p2 = v.find_last_not_of(Spaces);
	if (p1 == std::string_view::npos) {
		return {};
	}
	return v.substr(p1, p2 ? p2 - p1 + 1 : v.npos);
}

std::vector<std::string_view> util::string::split(std::string_view v, const std::string& delim) {
	if (v.empty()) return {};
	size_t pos = 0;
	std::vector<std::string_view> res;
	do {
		size_t newPos = v.find(delim, pos);
		if (newPos != std::string_view::npos) {
			res.push_back(v.substr(pos, newPos - pos));
			newPos += delim.size();
		}
		else {
			res.push_back(v.substr(pos));
		}
		pos = newPos;
	} while (pos != std::string_view::npos);
	return res;
}