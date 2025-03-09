// ReSharper disable CppClangTidyBugproneMacroParentheses
#pragma once

// helper of rule of 0/3/5
// only construct & deconstruct, no copy or assign allowed
// to cease clang tidy and double free issue 
// if custom destructor is set
// see: https://en.cppreference.com/w/cpp/language/rule_of_three
#define RULE_NO_COPY(ClassName) \
	ClassName(const ClassName& other) = delete; \
	ClassName(ClassName&& other) noexcept = delete; \
	ClassName& operator=(const ClassName& other) = delete; \
	ClassName& operator=(ClassName&& other) noexcept = delete