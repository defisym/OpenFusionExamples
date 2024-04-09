#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <functional>

#include "GeneralDefinition.h"

constexpr auto DEFAULT_FORMAT_RESERVE = 10;

struct FormatBasic {
	size_t start;
	//size_t end;

	size_t startWithNewLine;

	// copy POD format except the basic part
	template<typename Content>
		requires std::is_pod_v<Content>&& std::is_base_of_v<FormatBasic, Content>
	inline void CopyFormat(Content& content) {
		auto pData = reinterpret_cast<std::remove_reference_t<decltype(content)>*>(this + 1);
		memcpy(pData, &content, sizeof(content));
	}
};

struct FormatContext :FormatBasic {
	std::wstring_view controlStr;
	std::wstring_view controlParam;

	using ControlParams = std::vector<std::wstring_view>;
	ControlParams controlParams;

	bool bValidCommand = true;
	bool bEndOfRegion = false;
	bool bIgnoreFormat = false;
};

template<typename Child>
struct FormatParser {
	using CommandList = std::vector<std::wstring>;

	CommandList commandList;
	FormatContext* pCtx = nullptr;

	using ControlParams = std::vector<std::wstring_view>;
	ControlParams controlParams;

	using ParamParserCallback = std::function<void(std::wstring_view&)>;

	// parse by ',' use a while loop to get all params
	static inline bool ParseParamCore(std::wstring_view& paramParser, bool bReverse, const ParamParserCallback& callBack) {
		const auto start = bReverse
			? paramParser.find_last_of(L',')
			: paramParser.find_first_of(L',');

		if (start == std::wstring::npos) {
			callBack(paramParser);

			return false;
		}

		const auto before = paramParser.substr(0, start);
		const auto after = paramParser.substr(start + 1);

		auto paramStr = bReverse ? after : before;
		callBack(paramStr);

		paramParser = bReverse ? before : after;

		return true;
	}

	// from left
	static inline bool ParseParam(std::wstring_view& paramParser, const ParamParserCallback& callBack) {
		return ParseParamCore(paramParser, false, callBack);
	}

	// from right
	static inline bool ParseParamReverse(std::wstring_view& paramParser, const ParamParserCallback& callBack) {
		return ParseParamCore(paramParser, true, callBack);
	}

	FormatParser(const CommandList& commandList) {
		this->commandList = commandList;
	}

	inline bool CommandMatch(const std::wstring_view& command) {
		for (auto& cmd : commandList) {
			if (StringViewIEqu(command, cmd.c_str())) { return true; }
		}

		return false;
	}

	inline void ParseCommand(const std::wstring_view& command, const std::wstring_view& param, const FormatContext& ctx) {
		if (pCtx->bIgnoreFormat) { return; }
		if (!CommandMatch(command)) { return; }

		this->pCtx = &ctx;

		// CRTP
		static_cast<Child*>(this)->ParseCommand(param);
	}
};

template<typename Format>
struct FormatStack {
	//Format display = { };

	struct FormatDisplay :FormatBasic {
		Format display;
	};

	std::vector<FormatDisplay> displayFormat;
	std::vector<Format> displayStack;

	inline void Reserve() {
		this->displayFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->displayStack.reserve(DEFAULT_FORMAT_RESERVE);
	}

	inline void Clear(const Format& display) {
		this->displayStack.clear();
		this->displayStack.emplace_back(display);

		this->displayFormat.clear();
		this->displayFormat.emplace_back(FormatDisplay{ {0,0},displayStack.back() });
	}

	inline void UpdateFormat(const FormatContext& ctx, Format& content) {
		auto pLastFormat = &this->displayFormat.back();

		// if equal, replace last format
		if (ctx.start == pLastFormat->start) {
			pLastFormat->CopyFormat(content);
		}
		else {
			// copy first format display
			FormatDisplay newFormat = this->displayFormat.front();

			newFormat.start = ctx.start;
			newFormat.startWithNewLine = ctx.startWithNewLine;

			newFormat.CopyFormat(content);

			this->displayFormat.emplace_back(newFormat);
		}
	}

	// reset to the format at beginning
	inline void Reset(const FormatBasic& ctx) {
		// copy first format
		Format first = this->displayStack.front();

		this->displayStack.clear();
		this->displayStack.emplace_back(first);

		UpdateFormat(ctx, first);
	}

	// manage the stack
	// new element, call callback to update the one copied from last one
	// parse params there
	// or end of region, pop the stack, then update it
	inline void StackManager(const FormatContext& ctx, const std::function<Format&>& callBack) {
		if (!ctx.bEndOfRegion) {
			// clone one here
			Format newFormat = this->displayStack.back();

			callBack(newFormat);

			this->displayStack.emplace_back(newFormat);
		}
		else {
			// protect for default one
			if (this->displayStack.size() > 1) {
				this->displayStack.pop_back();
			}
		}

		UpdateFormat(ctx, this->displayStack.back());
	}

	template<typename ValueType>
	inline void DiffManager(FormatContext& ctx,
		ValueType oldValue,
		const std::function<ValueType(const std::wstring_view&)>& callBack) {
		auto& controlParam = ctx.controlParam;

		bool bAdd = false;
		bool bMinus = false;

		do {
			const auto& firstChar = controlParam.front();

			// escape
			if (firstChar == L'\\') {
				controlParam = controlParam.substr(1);

				break;
			}

			if (firstChar == L'+') {
				controlParam = controlParam.substr(1);
				bAdd = true;

				break;
			}

			if (firstChar == L'-') {
				controlParam = controlParam.substr(1);
				bAdd = true;
				bMinus = true;

				break;
			}
		} while (false);

		auto sizeDiff = callBack(controlParam);

		return !bAdd
			? sizeDiff
			: (bMinus ? -1 : +1) * sizeDiff + oldValue;
	}

	template<typename ValueType>
	inline bool Parser(FormatContext& ctx, const LPCWSTR pStr,
		const std::function<ValueType&(Format&)>& getToUpdate,
		const std::function<ValueType(const std::wstring_view&)>& updater) {
		if (!StringViewIEqu(ctx.controlStr, pStr)) { return false; }
		if (ctx.bIgnoreFormat) { return false; }

		StackManager(ctx, [&](Format& display) {
			auto& toUpdate = getToUpdate(display);

			// Reset
			if (StringViewIEqu(ctx.controlParam, L"!")) {
				toUpdate = getToUpdate(this->displayStack.front());

				return;
			}

			// Update
			toUpdate = DiffManager<ValueType>(ctx, toUpdate, updater);
			});

		return true;
	}
};

// struct RemarkDisplay {
// 	float remarkOffsetX = 0;
// 	float remarkOffsetY = 0;
// };

//template<typename T>
//struct FormatStack :FormatParser<FormatStack<T>> {
//	// default value
//	T display = { };
//
//	struct FormatDisplay :FormatBasic {
//		T display;
//	};
//
//	std::vector<FormatDisplay> displayFormat;
//	std::vector<T> displayStack;
//
//	inline void Reserve() {
//		this->displayFormat.reserve(DEFAULT_FORMAT_RESERVE);
//		this->displayStack.reserve(DEFAULT_FORMAT_RESERVE);
//	}
//
//	inline void Reset() {
//		this->displayStack.clear();
//		this->displayStack.emplace_back(this->display);
//
//		this->displayFormat.clear();
//		this->displayFormat.emplace_back(FormatDisplay{ {0,0},displayStack.back() });
//	}
//
//	FormatStack(const CommandList& commandList) :FormatParser<FormatStack<T>>(commandList) {}
//
//	// update format for last character
//	inline void UpdateFormat(T& content) {
//		auto pLastFormat = &displayFormat.back();
//
//		// if equal, replace last format
//		if (pCtx->start == pLastFormat->start) {
//			pLastFormat->CopyFormat(content);
//		}
//		else {
//			FormatDisplay newFormat = displayFormat.front();
//
//			newFormat.start = pCtx->start;
//			newFormat.startWithNewLine = pCtx->startWithNewLine;
//			newFormat.CopyFormat(content);
//
//			displayFormat.emplace_back(newFormat);
//		}
//	};
//
//	inline void StackManager(std::function<void(T&)> callBack) {
//		if (!pCtx->bEndOfRegion) {
//			// clone one here
//			T newFormat = displayStack.back();
//
//			callBack(newFormat);
//
//			displayStack.emplace_back(newFormat);
//		}
//		else {
//			// protect for default one
//			if (displayStack.size() > 1) {
//				displayStack.pop_back();
//			}
//		}
//
//		UpdateFormat(displayStack.back());
//	};
//
//	inline void ParseCommand(const std::wstring_view& controlParam) {
//		StackManager([&] (DWORD& dwDTFlags) {
//			// Reset
//			if (StringViewIEqu(controlParam, L"!")) {
//				dwDTFlags = displayStack.front();
//
//				return;
//			}
//
//			const auto& paramParser = controlParam;
//			controlParams.clear();
//
//			const auto newAlign = _stoi(paramParser);
//			dwDTFlags = (dwDTFlags & ~(DT_LEFT | DT_CENTER | DT_RIGHT)) | (newAlign);
//	});
//	}
//
//	inline FormatParser<FormatStack<T>>* GetParentPointer() {
//		return FormatParser<FormatStack<T>>*(this);
//	}
//};
//
//FormatStack<DWORD> dwStack({ L"Align" });
//auto p = dwStack.GetParentPointer();
//
//FormatContext ctx;
//p->ParseCommand(L"Align", L"0", ctx);
