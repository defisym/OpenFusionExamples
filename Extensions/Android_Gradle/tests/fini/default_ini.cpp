#include "IniCore.h"
#include <memory>

#if defined(FINI_PORTABLE_RUNTIME) || defined(NLOHMANN_JSON_VERSION_MAJOR)
#error The default INI API must not enable the mobile Runtime or JSON dependencies.
#endif

int main() {
    std::unique_ptr<CSimpleIniA> ini(FiniIni::Create<CSimpleIniA>());
    if (!FiniIni::Set(ini.get(), "section", "key", "value")) return 1;
    if (!FiniIni::Copy(ini.get(), "section", "copy", false)) return 2;
    if (FiniIni::Save(ini.get()).find("value") == std::string::npos) return 3;
    return FiniIni::Delete(ini.get(), "copy", "key") ? 0 : 4;
}
