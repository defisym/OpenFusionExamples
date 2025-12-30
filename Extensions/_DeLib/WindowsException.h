#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma comment(lib, "Kernel32.lib")

#include <format>
#include <string>

//------------
// Usage
//------------
//
//WindowsException::SetSEHFilter();
//WindowsException::SetVEHFilter();
//
//RaiseException(EXCEPTION_ARRAY_BOUNDS_EXCEEDED, 0, 0, nullptr);
//
//int* a = nullptr;
//auto b = *a;

namespace WindowsException {
#ifdef _DEBUG
    inline void RaiseException() {
        ::RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, nullptr);
    }
#endif

    constexpr auto UnknownException = L"Unknown Exception";

    inline constexpr const wchar_t* GetExceptionMessage(DWORD exceptionCode) {
        switch (exceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            return L"线程尝试读取或写入其没有相应访问权限的虚拟地址";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            return L"线程尝试访问超出边界的数组元素，基础硬件支持边界检查";
        case EXCEPTION_BREAKPOINT:
            return L"遇到断点";
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            return L"线程尝试读取或写入硬件上未提供对齐方式的数据。例如，必须在2字节边界上对齐16位值，4字节边界上的32位值等";
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            return L"浮点运算中的操作数之一是非正态。非规范值太小，无法表示为标准浮点值";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            return L"线程尝试将浮点值除以零的浮点除数";
        case EXCEPTION_FLT_INEXACT_RESULT:
            return L"浮点运算的结果不能完全表示为小数分数";
        case EXCEPTION_FLT_INVALID_OPERATION:
            return L"此异常表示此列表中未包含的任何浮点异常";
        case EXCEPTION_FLT_OVERFLOW:
            return L"浮点运算的指数大于相应类型允许的量级";
        case EXCEPTION_FLT_STACK_CHECK:
            return L"由于浮点操作，堆栈溢出或下溢";
        case EXCEPTION_FLT_UNDERFLOW:
            return L"浮点运算的指数小于相应类型允许的量级";
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            return L"线程尝试执行无效指令";
        case EXCEPTION_IN_PAGE_ERROR:
            return L"线程尝试访问不存在的页面，并且系统无法加载页面。例如，如果在通过网络运行程序时网络连接丢失，则可能会出现此异常";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            return L"线程尝试将整数值除以零的整数除数";
        case EXCEPTION_INT_OVERFLOW:
            return L"整数运算的结果导致执行结果的最大有效位";
        case EXCEPTION_INVALID_DISPOSITION:
            return L"异常处理程序将无效处置返回到异常调度程序。使用高级语言（如 C）的程序员不应遇到此异常";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            return L"发生不可连续异常后，线程尝试继续执行";
        case EXCEPTION_PRIV_INSTRUCTION:
            return L"线程尝试执行其操作在当前计算机模式下不允许的指令";
        case EXCEPTION_SINGLE_STEP:
            return L"跟踪陷阱或其他单指令机制指示已执行一个指令";
        case EXCEPTION_STACK_OVERFLOW:
            return L"线程已用到其堆栈";
        default:
            return UnknownException;
        }
    }
    
    inline bool ExceptionUnknown(DWORD exceptionCode) {
        return wcscmp(GetExceptionMessage(exceptionCode), UnknownException) == 0;               
    }

    inline std::wstring GetErrorMessage(DWORD msgID) {
        LPWSTR pBuf = nullptr;

        const auto bRet = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            msgID,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            reinterpret_cast<LPWSTR>(&pBuf),
            0,
            nullptr
        );

        if (bRet == 0) {
            auto m = GetErrorMessage(GetLastError());

            return {};
        }

        auto msg = std::wstring(pBuf);
        LocalFree(pBuf);

        return msg;
    }

    inline LONG __stdcall DefaultFilter(EXCEPTION_POINTERS* pExceptionInfo) {
        const auto code = pExceptionInfo->ExceptionRecord->ExceptionCode;
        const auto msg = GetExceptionMessage(code);

        MSGBOX(std::format(L"{:#x}\n{}", code, msg));

        return EXCEPTION_CONTINUE_SEARCH;
    }

    inline LPTOP_LEVEL_EXCEPTION_FILTER SetSEHFilter(LPTOP_LEVEL_EXCEPTION_FILTER filter = DefaultFilter) {
        return SetUnhandledExceptionFilter(filter);
    }
    inline PVOID SetVEHFilter(PVECTORED_EXCEPTION_HANDLER filter = DefaultFilter) {
        return AddVectoredExceptionHandler(1, filter);
    }
    inline void RemoveVEHFilter(PVOID filter) {
        RemoveVectoredExceptionHandler(filter);
    }
}