#pragma once

#include <WindowsCommon.h>

#include "nlohmann/json.hpp"

class JsonInterface {
private:
    using Json = nlohmann::json;
    using JsonData = decltype(Json::parse(static_cast<FILE*>(nullptr)));

    JsonData data;

    template<typename T>
    inline bool LoadCore(T* in) {
        if (in == nullptr) {
            return false;
        }

        try {
            data = Json::parse(in);
        } catch (...) {
            return false;
        }

        return true;
    }

public:
    inline bool Load(const wchar_t* pFileName) {
        FILE* fp = nullptr;
        _wfopen_s(&fp, pFileName, L"rb");

        return LoadCore(fp);
    }

    inline bool Load(const char* pBuf) {
        return LoadCore(pBuf);
    }

    inline const JsonData& Get() {
        return data;
    }

    static inline bool Contains(const JsonData& data, std::string key) {
        return data.contains(key);
    }
	static inline bool Contains(const JsonData& data, std::wstring key) {
        return Contains(data, ConvertWStrToStr(key));
    }

    static inline auto GetData(const JsonData& data, const std::string& key) {
        return data[key];
    }

    template<typename T>
    static inline bool GetData(const JsonData& data, const std::string& key, T& output) {
        const auto bContain = Contains(data, key);

        if constexpr (std::is_pointer_v<T>) {
            using TClass = std::remove_pointer_t<T>;

            if (bContain) {
                output = new TClass(data[key]);

                return true;
            }

            if constexpr (std::is_default_constructible_v<TClass>) {
                output = new TClass();
            }

            return false;
        }
        else {
            if (bContain) {
                if constexpr (std::is_same_v<std::wstring, T>)
                    output = ConvertStrToWStr(data[key].get<std::string>());
                else
                    output = data[key].get<T>();

                return true;
            }

            return false;
        }
    }

    template<typename T>
    static inline bool GetData(const JsonData& data, const std::wstring& key, T& output) {
        return GetData(data, ConvertWStrToStr(key), output);
    }
};