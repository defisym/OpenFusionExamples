#pragma once

#include "JsonInterface.h"

#include "Encryption.h"
#include "GeneralDefinition.h"

inline bool LoadJsonFile(JsonInterface& JI, const LPCWSTR pFileName, const LPCTSTR pKey = L"") {
    JI.SetComment(true);

    auto ret = true;

    if (StrEmpty(pKey)) {
        ret = JI.Load(pFileName);
    }
    else {
        Encryption E;
        E.GenerateKey(pKey);
        E.DecryptFile(pFileName);

        ret = JI.Load(E.GetOutputStr());
    }

    if (!ret) {
#if !defined(RUN_ONLY)
        MSGBOX(L"Json file load error");
#endif
    }

    return ret;
}