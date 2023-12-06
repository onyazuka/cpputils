#pragma once
#include <type_traits>
#include <variant>

namespace util::traits {
	
	template<typename T, typename Args>
	struct IsOneOfVariants : std::false_type {

	};

	template<typename T, typename...Args>
		requires (std::same_as<T, Args> || ...)
	struct IsOneOfVariants<T, std::variant<Args...>> : std::true_type {

	};

	template<typename T>
	struct IsTuple : std::false_type {

	};

	template<typename... Args>
	struct IsTuple<std::tuple<Args...>> : std::true_type {

	};

}