// ReSharper disable CppInconsistentNaming
#pragma once

#include <cassert>

#include <vector>
#include <string>
#include <format>

template<typename T>
class FormatByVector {
private:
	std::wstring fmt;
	std::vector<T> params;

	template <std::size_t... Indices>
	inline static auto VectorToFormatArgsHelper(const std::vector<T>& v, std::index_sequence<Indices...>) {
		return std::make_wformat_args(v[Indices]...);
	}

	template <std::size_t N>
	inline static auto VectorToFormatArgs(const std::vector<T>& v) {
		assert(v.size() >= N);
		return VectorToFormatArgsHelper(v, std::make_index_sequence<N>());
	}

public:
	explicit FormatByVector(const std::wstring& fmt) {
		this->fmt = fmt;
	}

	template <class... Args>
	inline void AddParam(Args&&... vals) {
		this->params.emplace_back(std::forward<Args>(vals)...);
	}

	inline void SetParams(const std::vector<T>& newParams) {
		this->params = newParams;
	}

private:
	template <std::size_t N>
	inline std::wstring GetFormatStringHelper() const {
		try {
			return std::vformat(fmt, VectorToFormatArgs<N>(this->params));
		} catch (std::exception& e) {
			const auto error = e.what();

			//return std::wstring{ L"failed" };
			return this->fmt;
		}
	}

#define GetFormatCaseHelper(X) GetFormatStringHelper<X>()
#define GetFormatCase(X) case X: return GetFormatCaseHelper(X)

#define GetFormatCase_1(X) GetFormatCase(X)
#define GetFormatCase_2(X) GetFormatCase((X)-1); GetFormatCase_1(X)
#define GetFormatCase_4(X) GetFormatCase_2((X)-2); GetFormatCase_2(X)
#define GetFormatCase_8(X) GetFormatCase_4((X)-4); GetFormatCase_4(X)
#define GetFormatCase_16(X) GetFormatCase_8((X)-8); GetFormatCase_8(X)
#define GetFormatCase_32(X) GetFormatCase_16((X)-16); GetFormatCase_16(X)
#define GetFormatCase_64(X) GetFormatCase_32((X)-32); GetFormatCase_32(X)
#define GetFormatCase_128(X) GetFormatCase_64((X)-64); GetFormatCase_64(X)

#define GetFormatCaseGen(X) GetFormatCase_##X((X)-1); GetFormatCase(X)

public:
	inline std::wstring GetFormatString() const {
		switch (this->params.size()) {
			GetFormatCaseGen(128);
		default:
			return GetFormatCaseHelper(0);
		}
	}
};