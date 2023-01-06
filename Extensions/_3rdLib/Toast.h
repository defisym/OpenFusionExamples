// https://github.com/mohabouje/WinToast

#include "WinToast/wintoastlib.h"

using namespace WinToastLib;

constexpr auto WinToastError_InCompatible = 1;
constexpr auto WinToastError_InitFailed = 2;

constexpr auto WinToastFlags_Default = 0b0000000000000000;

constexpr auto WinToastFlags_ImmediatePush = 0b0000000000000001;

class WinToastHandler : public WinToastLib::IWinToastHandler {
public:
    WinToastHandler() {}
    ~WinToastHandler() {}

    // Public interfaces
    void toastActivated() const override {
        return;
    }
    void toastActivated(int actionIndex) const {
        return;
    }
    void toastDismissed(WinToastDismissalReason state) const override {
        return;
    }
    void toastFailed() const override {
        return;
    }
};

class WinToastHelper {
private:
    bool bInit = false;
    bool bAvailable = false;
    WinToastHandler* pToastHandler = nullptr;

    //std::vector<INT64> notifications;
    bool bImmediatePush = false;
    LPRDATA rdPtr = nullptr;

    inline void Init() {
        this->bAvailable = false;

        auto rhPtr = rdPtr->rHo.hoAdRunHeader;
        auto rhApp = rhPtr->rhApp;

#if !defined(RUN_ONLY)
        auto appName = L"Edrt";
        auto companyName = L"Clicteam";
        auto productName = L"Fusion";

        auto version = _itos(rhApp->m_miniHdr.gaPrdBuild);
#else
        auto appName = rhApp->m_name;
        auto companyName = rhApp->m_copyright == nullptr
            ? appName
            : rhApp->m_copyright;
        auto productName = appName;

        auto version = _itos((rhApp->m_miniHdr.gaVersion << 16) + rhApp->m_miniHdr.gaSubVersion);
#endif

        WinToast::instance()->setAppName(appName);
        const auto aumi =
            WinToast::configureAUMI(companyName
                , productName
                , appName
                , version);

        WinToast::instance()->setAppUserModelId(aumi);

        if (!WinToast::instance()->initialize()) {
            //throw WinToastError_InitFailed;
            return;
        }

        this->bInit = true;
        this->bAvailable = true;
    }

public:
    WinToastHelper(LPRDATA rdPtr, DWORD flags = WinToastFlags_Default) {
        this->bAvailable = false;

        if (!IsCompatible()) {
            //throw WinToastError_InCompatible;
            return;
        }

        this->rdPtr = rdPtr;     
        this->pToastHandler = new WinToastHandler;
        this->bAvailable = true;
    };

    ~WinToastHelper() {
        // exception of access violation if notification still exists when closing
        //for (auto& ID : notifications) {
        //    WinToast::instance()->hideToast(ID);
        //}

        WinToast::instance()->clear();

        delete pToastHandler;
        pToastHandler = nullptr;
    };

    inline bool IsCompatible() {
        return WinToast::isCompatible();
    }

    inline void SetFlag(DWORD flags = WinToastFlags_Default) {
        bImmediatePush = flags & WinToastFlags_ImmediatePush;
    }

    // Toast
    inline bool ShowToast(std::wstring&& title, std::wstring&& content) {
        if (!bInit) {
            Init();
        }

        if (!bAvailable) {
            return false;
        }

        WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text02);

        templ.setTextField(title, WinToastTemplate::FirstLine);
        templ.setTextField(content, WinToastTemplate::SecondLine);

        if (bImmediatePush) {
            WinToast::instance()->clear();
        }

        WinToast::WinToastError error;
        auto ret = WinToast::instance()->showToast(templ, pToastHandler, &error);

        bool bValid = !(ret < 0);

        //if (bValid) {
        //    notifications.emplace_back(ret);
        //}

        return bValid;
    }
};