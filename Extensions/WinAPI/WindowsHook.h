#pragma once

// reference:
// https://stackoverflow.com/questions/55532578/winapi-check-if-window-resizing-has-been-finished

//struct tagRDATA;
//typedef tagRDATA RUNDATA;
//typedef RUNDATA* LPRDATA;

#include <functional>
#pragma comment(lib,"User32.lib")

class WindowsHook {
    using HookCallback = std::function<void()>;

    inline static WindowsHook* pThis;

    HWND hwnd = nullptr;
    HHOOK hHook = nullptr;
    HookCallback cb = nullptr;
    bool bWindowResizing = false;

   
public:
    explicit WindowsHook(HINSTANCE hmod, HWND hwnd, const HookCallback& cb) {
        WindowsHook::pThis = this;

        this->hwnd = hwnd;
        this->cb = cb;
        this->hHook = SetWindowsHookEx(WH_GETMESSAGE,
            [] (int code, WPARAM wParam, LPARAM lParam)->LRESULT {
                if (pThis != nullptr) {
                    return pThis->HookProc(code, wParam, lParam);
            	}

                return CallNextHookEx(nullptr, code, wParam, lParam);
            },
            hmod, 0);

#ifdef _DEBUG
        const auto err = GetLastError();
        OutputDebugStringA(std::format("{}\r\n", err).c_str());
#endif
    }
    ~WindowsHook() {
        if (hHook != nullptr) {
            UnhookWindowsHookEx(hHook);
        }
    }

    //https://learn.microsoft.com/en-us/windows/win32/winmsg/getmsgproc
    inline LRESULT HookProc(int code, WPARAM wParam, LPARAM lParam) {
        do {
            if (code < 0) {
                break;
            }

            const MSG* lpmsg = reinterpret_cast<MSG*>(lParam);
        	//Main hwnd passed in
            if (lpmsg->hwnd != hwnd) {
                break;
            }

#ifdef _DEBUG
            OutputDebugStringA(std::format("Info: {}\r\n", lpmsg->message).c_str());
#endif

            if (lpmsg->message == WM_ENTERSIZEMOVE && !bWindowResizing) {
#ifdef _DEBUG
                OutputDebugStringA("Resizing\r\n");
#endif
            	bWindowResizing = true;
            }
            else if (lpmsg->message == WM_EXITSIZEMOVE && bWindowResizing) {
#ifdef _DEBUG
                OutputDebugStringA("Resize Complete\r\n");
#endif
                bWindowResizing = false;

                if (cb != nullptr) {
                    cb();
                }
            }
        } while (false);

        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
};

