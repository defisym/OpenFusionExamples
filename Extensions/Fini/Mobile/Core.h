#pragma once
// Computational core only; Fusion objects and event dispatch stay in each runtime.
#define SI_NO_CONVERSION
#include "../../_3rdLib/simpleini/SimpleIni.h"
#include "../../_3rdLib/nlohmann/single_include/nlohmann/json.hpp"
#include "../../_DeLib/Encryption/CAPI.h"
#include <array>
#include <filesystem>
#include <memory>
#include <vector>
#include <cstring>
#include <limits>
#include <stdexcept>
#include "../../_DeLib/UnicodeConversion.h"
#include "../../_3rdLib/Base64.h"
#include "../../_3rdLib/Compress/zlibInterface.h"
#include "../IniCore.h"
#include <fstream>
#include <cmath>
#include "../../_DeLib/StrNum.h"
#include <codecvt>
#include <locale>

namespace fini {
using Json = nlohmann::json;
inline void check(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
// Reuse the Windows extension's C++ codecs, not platform copies.
inline std::string base64(const std::string& data) {
    check(data.size()<=UINT_MAX,"Base64 input too large");
    return Base64<std::string>{}.base64_encode(reinterpret_cast<const unsigned char*>(data.data()),data.size());
}
inline std::string unbase64(const std::string& text) {
    auto bytes=Base64<std::string>{}.base64_decode(text);
    return std::string(bytes.begin(),bytes.end());
}
inline std::string compress(const std::string& data) {
    std::string out;check(ZLIBI_Compress(data,out),"Compress failed");return out;
}
inline std::string decompress(const std::string& data) {
    std::string out;check(ZLIBI_UnCompress(data.data(),data.size(),out,4,64*1024*1024),"Invalid or oversized zlib data");return out;
}
inline std::string crypt(const std::string& data,const std::string& password,bool encrypt) {
    // Call the common runtime C ABI; no EVP/core cipher implementation in Fini.
    const auto units=to_utf16_string(password);
    std::vector<uint8_t> encoded;
    for(auto unit:units){encoded.push_back(static_cast<uint8_t>(unit));encoded.push_back(static_cast<uint8_t>(unit>>8));}
    std::array<uint8_t,16> key{},iv{};
    check(Encryption_DeriveKeyAndIv(encoded.data(),encoded.size(),key.data(),iv.data())==ENCRYPTION_OK,"Derive encryption material");
    check(data.size()<=(std::numeric_limits<size_t>::max)()-16,"Encryption input too large");
    std::string result(data.size()+16,'\0');size_t written=0;
    auto transform=encrypt?Encryption_EncryptBuffer:Encryption_DecryptBuffer;
    check(transform(key.data(),iv.data(),reinterpret_cast<const uint8_t*>(data.data()),data.size(),reinterpret_cast<uint8_t*>(result.data()),result.size(),&written)==ENCRYPTION_OK,"Shared encryption operation failed");
    result.resize(written);return result;
}
inline std::string read(const std::string& path,const std::string& key) {
    std::ifstream in(std::filesystem::path(std::u8string(reinterpret_cast<const char8_t*>(path.data()),path.size())),std::ios::binary); check(bool(in),"Open INI file");
    std::string data((std::istreambuf_iterator<char>(in)),{}); check(!in.bad(),"Read INI file");
    return key.empty()?data:crypt(data,key,false);
}
inline void write(const std::string& path,const std::string& key,const std::string& data) {
    check(!path.empty(),"Empty INI path"); auto bytes=key.empty()?data:crypt(data,key,true);
    std::ofstream out(std::filesystem::path(std::u8string(reinterpret_cast<const char8_t*>(path.data()),path.size())),std::ios::binary|std::ios::trunc); check(bool(out),"Open INI output");
    out.write(bytes.data(),bytes.size()); out.close(); check(bool(out),"Write INI output");
}
inline void put32(std::string& out,uint32_t n) { for(int i=0;i<4;++i) out+=static_cast<char>(n>>(8*i)); }
inline uint32_t get32(const std::string& in,size_t& p) { check(p+4<=in.size(),"Truncated object data"); uint32_t n=0; for(int i=0;i<4;++i) n|=uint32_t(static_cast<unsigned char>(in[p++]))<<(8*i); return n; }
inline std::string packValues(const Json& data) {
    std::string out; put32(out,data.at("flags").get<uint32_t>());
    for(const auto& v:data.at("values")) {
        int type=v.at("type"); check(type==0||type==2,"Invalid numeric value type"); put32(out,type); put32(out,0);
        if(type==0) { put32(out,v.at("value").get<int32_t>()); put32(out,0); }
        else { double d=v.at("value"); uint64_t bits; std::memcpy(&bits,&d,8); put32(out,bits); put32(out,bits>>32); }
    }
    for(const auto& s:data.at("strings")) {
        auto units=to_utf16_string(s.get<std::string>());
        for(auto c:units) { out+=static_cast<char>(c); out+=static_cast<char>(c>>8); } out.append(2,'\0');
    }
    return base64(out);
}
inline Json unpackValues(const std::string& text,size_t values,size_t strings) {
    auto in=unbase64(text); size_t p=0; Json result={{"flags",get32(in,p)},{"values",Json::array()},{"strings",Json::array()}};
    check(values<=4096 && strings<=4096,"Too many alterable values");
    for(size_t i=0;i<values;++i) {
        auto type=get32(in,p); get32(in,p); auto low=get32(in,p), high=get32(in,p); check(type==0||type==2,"Invalid numeric value type");
        if(type==0) result["values"].push_back({{"type",0},{"value",static_cast<int32_t>(low)}});
        else { uint64_t bits=uint64_t(low)|(uint64_t(high)<<32); double d; std::memcpy(&d,&bits,8); check(std::isfinite(d),"Nonfinite value"); result["values"].push_back({{"type",2},{"value",d}}); }
    }
    for(size_t i=0;i<strings;++i) {
        std::u16string units;
        for(;;) { check(p+2<=in.size(),"Truncated UTF-16 string"); auto c=static_cast<unsigned char>(in[p])|(static_cast<unsigned char>(in[p+1])<<8); p+=2; if(!c) break; units+=static_cast<char16_t>(c); }
        result["strings"].push_back(std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t>{}.to_bytes(units));
    }
    check(p==in.size(),"Object data count mismatch"); return result;
}
class State {
    std::unique_ptr<CSimpleIniA> ini_,localization_;
public:
    bool autoSave=true,modified=false;
    std::string path,key,language;
    static std::unique_ptr<CSimpleIniA> makeIni() { return std::unique_ptr<CSimpleIniA>(FiniIni::Create<CSimpleIniA>()); }
    bool initialized() const { return bool(ini_); }
    void reset() { ini_=makeIni(); modified=false; }
    void release() { ini_.reset(); modified=false; }
    std::string save() const { return FiniIni::Save(ini_.get()); }
    void flush() { if(ini_&&autoSave&&modified&&!path.empty()) {write(path,key,save());modified=false;} }
    void load(const std::string& data) { auto next=makeIni(); check(next->LoadData(data.data(),data.size())>=0,"Parse INI"); flush(); ini_=std::move(next); modified=false; }
    void loadFile(const std::string& file,const std::string& password) { flush(); auto bytes=read(file,password); load(bytes); if(autoSave){path=file;key=password;} }
    void loadLocalization(const std::string& file,const std::string& password) { localization_=makeIni(); if(!file.empty()) {auto bytes=read(file,password); check(localization_->LoadData(bytes.data(),bytes.size())>=0,"Parse localization");} }
    void localizeFrom(const std::string& data) { localization_=makeIni(); localization_->LoadData(data.data(),data.size()); }
    std::string raw(const std::string& section,const std::string& item) const { return ini_&&!section.empty()?ini_->GetValue(section.c_str(),item.c_str(),""):""; }
    std::string get(const std::string& section,const std::string& item) const { auto value=raw(section,item); if(localization_&&!language.empty()) {std::string translated=localization_->GetValue(language.c_str(),value.c_str(),""); if(!translated.empty()) return translated;} return value; }
    Json names(const std::string* section=nullptr) const { Json result=Json::array(); if(!ini_)return result; CSimpleIniA::TNamesDepend names; if(section)ini_->GetAllKeys(section->c_str(),names);else ini_->GetAllSections(names); for(const auto& n:names)result.push_back(n.pItem); return result; }
    Json request(const std::string& op,const Json& a=Json::array()) {
        auto s=[&](size_t i){return a.at(i).get<std::string>();};
        if(op=="initialized")return initialized();
        if(op=="fileData"||op=="localBytes"){auto data=unbase64(s(0));auto password=s(1);if(!password.empty())data=crypt(data,password,false);if(op=="fileData"){load(data);if(autoSave){path=s(2);key=password;}}else localizeFrom(data);return nullptr;}
        if(op=="new") { reset(); return nullptr; } if(op=="release") { release(); return nullptr; }
        if(op=="save")return save(); if(op=="load"){load(s(0));return nullptr;}
        if(op=="fileLoad"){loadFile(s(0),s(1));return nullptr;}
        if(op=="fileSave"){if(ini_)write(s(0),s(1),save());return nullptr;}
        if(op=="flush"){flush();return nullptr;}
        if(op=="autoOn"){autoSave=true;path=s(0);key=s(1);return nullptr;}
        if(op=="autoOff"){autoSave=false;return nullptr;}
        if(op=="setNumber")return request("set",{s(0),s(1),std::format("{}",a.at(2).get<float>())});
        if(op=="set") {if(!ini_)reset();auto sec=s(0),item=s(1),val=s(2);if(!sec.empty()&&!item.empty())modified=FiniIni::Set(ini_.get(),sec.c_str(),item.c_str(),val.c_str())||modified;return nullptr;}
        if(op=="delete") {auto sec=s(0),item=s(1);if(ini_&&!sec.empty())modified=FiniIni::Delete(ini_.get(),sec.c_str(),item.empty()?nullptr:item.c_str())||modified;return nullptr;}
        if(op=="copy") {auto src=s(0),dst=s(1);if(ini_&&!src.empty()&&!dst.empty()) {modified=FiniIni::Copy(ini_.get(),src.c_str(),dst.c_str(),a.at(2).get<bool>())||modified;}return nullptr;}
        if(op=="get")return get(s(0),s(1));
        if(op=="number") {auto v=_stof(raw(s(0),s(1)));return std::isfinite(v)?Json(v):Json(0);}
        if(op=="hasSection") {auto sec=s(0);return ini_&&!sec.empty()&&ini_->SectionExists(sec.c_str());}
        if(op=="hasItem") {auto sec=s(0),item=s(1);return ini_&&!sec.empty()&&!item.empty()&&ini_->KeyExists(sec.c_str(),item.c_str());}
        if(op=="sections")return names(); if(op=="items"){auto sec=s(0);return names(&sec);}
        if(op=="localFile"){loadLocalization(s(0),s(1));return nullptr;}
        if(op=="localData"){localizeFrom(s(0));return nullptr;} if(op=="language"){language=s(0);return nullptr;}
        if(op=="encode")return base64(a.at(1).get<bool>()?compress(s(0)):s(0));
        if(op=="decode"){auto data=unbase64(s(0));return a.at(1).get<bool>()?decompress(data):data;}
        if(op=="packValues")return packValues(a.at(0));
        if(op=="unpackValues")return unpackValues(s(0),a.at(1),a.at(2));
        if(op=="packPosition"){std::string bytes;put32(bytes,a.at(0).get<int32_t>());put32(bytes,a.at(1).get<int32_t>());return base64(bytes);}
        if(op=="unpackPosition"){auto bytes=unbase64(s(0));check(bytes.size()==8,"Invalid position data");size_t p=0;auto x=static_cast<int32_t>(get32(bytes,p)),y=static_cast<int32_t>(get32(bytes,p));return Json::array({x,y});}
        throw std::runtime_error("Unknown Fini operation");
    }
};
} // namespace fini
