#pragma once

#include "StringTraits.h"

#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// ------------------------------------------
// basic split
// ------------------------------------------

// split string by delimiter
template<CharConcept T>
inline void SplitStringCore(const std::basic_string_view<T>& input,
	const T delimiter,
	const std::function<void(const std::basic_string_view<T>&)>& callBack) {
	//https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
	size_t start = input.find_first_not_of(delimiter);
	size_t end = start;

	while (start != std::basic_string_view<T>::npos) {
		end = input.find(delimiter, start);
		callBack({ input.data() + start,
			end == std::basic_string_view<T>::npos
			? input.length() - start
			: end - start });
		start = input.find_first_not_of(delimiter, end);
	}
}

// split string by delimiter, return but will terminate if callback returns false
template<CharConcept T>
inline void SplitStringUntilCore(const std::basic_string_view<T>& input,
    const T delimiter,
    const std::function<bool(const std::basic_string_view<T>&)>& callBack) {
    //https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
    size_t start = input.find_first_not_of(delimiter);
    size_t end = start;

    while (start != std::basic_string_view<T>::npos) {
        end = input.find(delimiter, start);
        if (!callBack({ input.data() + start,
            end == std::basic_string_view<T>::npos
            ? input.length() - start
            : end - start })) {
            break;
        }

        start = input.find_first_not_of(delimiter, end);
    }
}

template<typename RET, CharConcept T>
inline std::vector<RET> SplitString(const std::basic_string<T>& input,
	const T delimiter,
	std::function<RET(const std::basic_string_view<T>&)> callBack) {
	std::vector<RET> resultList;

	SplitStringCore<T>(input, delimiter,
		[&] (const std::basic_string_view<T>& item) {
			resultList.emplace_back(std::move(callBack(item)));
		});

	return resultList;
}

template<CharConcept T>
inline std::vector<std::basic_string<T>> SplitString(const std::basic_string<T>& input, const T delimiter) {
	return SplitString<std::basic_string<T>, T>(input, delimiter,
		[] (const std::basic_string_view<T>& item) {
			std::basic_string<T> obj{ item.data(),item.length() };

			return obj;
		});
}
