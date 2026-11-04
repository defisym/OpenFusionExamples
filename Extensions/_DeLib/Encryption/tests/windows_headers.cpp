// Simulate existing Windows plugin TUs that include the SDK before Encryption.h.
#ifdef _WIN32
#include <windows.h>
#include "Encryption.h"
#include <stdexcept>
#endif
void test_windows_headers() {
#ifdef _WIN32
    Encryption encryption;
    BYTE bytes[] = {1, 2, 3};
    DWORD size = 3;
    encryption.SetEncryptData(bytes, size);
    PBYTE input = encryption.GetInputData();
    DWORD actual = encryption.GetInputDataLength();
    LPCWSTR hash = encryption.GetHash();
    if (!input || actual != size || !hash || !encryption.Encrypt())
        throw std::runtime_error("Existing Windows SDK callers must keep working");
#endif
}
