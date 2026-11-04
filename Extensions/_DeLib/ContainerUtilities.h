#pragma once

#include <algorithm>
#include <vector>

// ------------------------------------------
// Insert sort
// ------------------------------------------

template<typename T>
typename std::vector<T>::iterator
InsertSortedUpperBound(std::vector<T>& vec, T const& item) {
	return vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
}
template<typename T, typename Pred>
typename std::vector<T>::iterator
InsertSortedUpperBound(std::vector<T>& vec, T const& item, Pred pred) {
	return vec.insert(std::upper_bound(vec.begin(), vec.end(), item, pred), item);
}
template<typename T>
typename std::vector<T>::iterator
InsertSortedLowerBound(std::vector<T>& vec, T const& item) {
	return vec.insert(std::lower_bound(vec.begin(), vec.end(), item), item);
}
template<typename T, typename Pred>
typename std::vector<T>::iterator
InsertSortedLowerBound(std::vector<T>& vec, T const& item, Pred pred) {
	return vec.insert(std::lower_bound(vec.begin(), vec.end(), item, pred), item);
}
