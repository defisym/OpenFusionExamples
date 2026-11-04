#pragma once
#include <string>
#include "../_3rdLib/simpleini/SimpleIni.h"

// Runtime-independent operations extracted from Fini/Main.cpp and Func.h.
// Windows uses wide-character SimpleIni; mobile uses its UTF-8 interface.
namespace FiniIni {
template<class Ini> Ini* Create() {auto* ini=new Ini;ini->SetUnicode();return ini;}
template<class Ini> std::string Save(const Ini* ini) {std::string out;if(ini)ini->Save(out);return out;}
template<class Ini,class Char> bool Set(Ini* ini,const Char* section,const Char* item,const Char* value) {
    if(!ini||!section||!*section||!item||!*item)return false;
    const auto result=ini->SetValue(section,item,value);
    return result==SI_UPDATED||result==SI_INSERTED;
}
template<class Ini,class Char> bool Delete(Ini* ini,const Char* section,const Char* item) {
    return ini&&section&&*section&&ini->Delete(section,item,true);
}
template<class Ini,class Char> bool Copy(Ini* ini,const Char* source,const Char* destination,bool removeSource) {
    if(!ini||!source||!*source||!destination||!*destination)return false;
    typename Ini::TNamesDepend names;ini->GetAllKeys(source,names);
    for(const auto& name:names)ini->SetValue(destination,name.pItem,ini->GetValue(source,name.pItem));
    if(removeSource)ini->Delete(source,nullptr,true);
    return !names.empty();
}
}
