#include "GeneralDefinition.h"
#include <iostream>
int main() {
    const std::string gbk="\xD6\xD0\xCE\xC4";
    if (to_byte_string(L"中文",CP_ACP)!=gbk || to_wide_string(gbk,CP_ACP)!=L"中文"
            || to_byte_string(L"中文",CP_UTF8)==gbk) { return 1; }
    bool unicode = false;
    std::wstring wide; std::string bytes;
    if (!LoadData(wide,gbk.data(),gbk.size(),unicode) || unicode || wide!=L"中文"
            || !SaveData(bytes,wide.data(),wide.size(),false) || bytes!=gbk) { return 1; }
    std::cout << "PASS: explicit ACP override and independent UTF8 codepage\n";
}
