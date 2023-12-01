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

std::string util::string::v2str(std::string_view v) {
	return std::string(v.data(), v.size());
}

util::string::Splitter::SplitIterator::SplitIterator(pointer ptr, const std::string& delim, size_t _pos)
	: sv{ ptr }, delim{ delim }, prevPos{ _pos }, pos{ _pos }, count{ 0 }
{
	++(*this);
}

util::string::Splitter::SplitIterator::reference util::string::Splitter::SplitIterator::operator*() const {
	if (prevPos == sv.npos) {
		return sv.substr(0, 0);
	}
	return sv.substr(prevPos, pos - prevPos);
}

util::string::Splitter::SplitIterator::pointer util::string::Splitter::SplitIterator::operator->() {
	if (prevPos == sv.npos) {
		return sv.substr(0, 0);
	}
	return sv.substr(prevPos, pos - prevPos);
}

// Prefix increment
util::string::Splitter::SplitIterator util::string::Splitter::SplitIterator::operator++() {
	if (pos == sv.npos) {
		prevPos = pos;
		return *this;
	}
	prevPos = pos;
	if (count) {
		prevPos += delim.size();
	}
	pos = sv.find(delim, prevPos);
	++count;
	return *this;
}

// Postfix increment
util::string::Splitter::SplitIterator util::string::Splitter::SplitIterator::operator++(int) {
	SplitIterator tmp = *this;
	++(*this);
	return tmp;
}

util::string::Splitter::Splitter(std::string_view _sv, const std::string& _delim)
	: sv{ _sv }, delim{ _delim }
{

}

util::string::Splitter::Splitter(std::string_view _sv, std::string&& _delim)
	: sv{ _sv }, delim{ std::move(_delim) }
{

}

util::string::Splitter::SplitIterator util::string::Splitter::begin() {
	return SplitIterator(sv, delim, 0);
}

util::string::Splitter::SplitIterator util::string::Splitter::end() {
	return SplitIterator(sv, delim, sv.npos);
}