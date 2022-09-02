// https://www.codeproject.com/Articles/5286393/Cplusplus-Windows-Toast-Notification
// https://github.com/mohabouje/WinToast

#include "WinToast/wintoastlib.h"

using namespace WinToastLib;

class WinToastHandler : public WinToastLib::IWinToastHandler
{
public:
    WinToastHandler(CDialogEx* pDialog) : m_pDialog(pDialog) {}
    // Public interfaces
    void toastActivated() const override {}
    void toastActivated(int actionIndex) const override {
        wchar_t buf[250];
        swprintf_s(buf, L"Button clicked: %d", actionIndex);
        m_pDialog->MessageBox(buf, L"info", MB_OK);
    }
    void toastDismissed(WinToastDismissalReason state) const override {}
    void toastFailed() const override {}
private:
    CDialogEx* m_pDialog;
};