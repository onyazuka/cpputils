#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace util::string
{
	std::string_view strip(std::string_view v);
	std::vector<std::string_view> split(std::string_view v, const std::string& delim);
	std::string v2str(std::string_view v);

	class Splitter {
	public:

		class SplitIterator {
		public:
			using iterator_category = std::input_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = std::string_view;
			using pointer = std::string_view;
			using reference = std::string_view;

			SplitIterator(pointer ptr, const std::string& delim, size_t _pos);
			reference operator*() const;
			pointer operator->();
			// Prefix increment
			SplitIterator operator++();
			// Postfix increment
			SplitIterator operator++(int);

			inline friend bool operator== (const SplitIterator& a, const SplitIterator& b) {
				return (a.pos == b.pos) && (a.prevPos == b.prevPos);
			};
			inline friend bool operator!= (const SplitIterator& a, const SplitIterator& b) {
				return !(a == b);
			};
		private:
			pointer sv;
			const std::string& delim;
			size_t prevPos;
			size_t pos;
			size_t count;
		};

		Splitter(std::string_view _sv, const std::string& _delim);
		Splitter(std::string_view _sv, std::string&& _delim);
		Splitter(const Splitter&) = delete;
		const Splitter& operator=(const Splitter&) = delete;

		SplitIterator begin();
		SplitIterator end();

	private:
		std::string_view sv;
		std::string delim;
	};
}
