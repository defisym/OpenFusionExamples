#include "IniCore.h"
#include <iostream>
using fini::check;
using fini::Json;
template<class F> void rejects(F f){bool caught=false;try{f();}catch(const std::exception&){caught=true;}check(caught,"Expected failure");}
int main(){
    fini::State state;
    check(!state.request("hasSection",{"A"}).get<bool>(),"Uninitialized section");
    check(!state.request("hasItem",{"A","B"}).get<bool>(),"Uninitialized item");
    check(state.save().empty(),"Uninitialized save");
    state.request("set",{"Section","Item","你好 🌍"});
    check(state.get("section","ITEM")=="你好 🌍","INI case/UTF8");
    check(state.modified,"Set modified");
    state.request("set",{"Section","float","-15.2"});
    check(std::abs(state.request("number",{"SECTION","float"}).get<double>()+15.2)<0.00001,"Float");
    state.request("set",{"Section","text","nonnumeric"});
    check(state.request("number",{"Section","text"})==0,"Nonnumeric");
    check(state.request("number",{"missing","none"})==0,"Missing number");
    state.request("copy",{"Section","Copy",false});
    check(state.get("Copy","Item")=="你好 🌍","Copy");
    state.request("delete",{"Copy","Item"});check(!state.request("hasItem",{"Copy","Item"}).get<bool>(),"Delete item");
    state.request("delete",{"Copy",""});check(!state.request("hasSection",{"Copy"}).get<bool>(),"Delete section");
    auto snapshot=state.request("sections");state.release();check(!snapshot.empty(),"Owned iteration snapshot");
    auto original=std::string("[Mixed]\r\nvalue = 中文 😀\r\nnum=1.00000\r\n");state.load(original);
    fini::State other;other.load(state.save());check(other.get("mixed","VALUE")=="中文 😀","Save/load");
    for(bool zipped:{false,true}){auto encoded=state.request("encode",{state.save(),zipped});auto decoded=state.request("decode",{encoded,zipped}).get<std::string>();check(decoded==state.save(),"Base64 roundtrip");}
    check(fini::base64("hello")=="aGVsbG8=","Base64 known answer");
    check(fini::unbase64("AAECA/8=")==std::string("\0\1\2\3\xff",5),"Base64 binary known answer");
    rejects([]{fini::unbase64("bad");});rejects([]{fini::unbase64("ab==");});rejects([]{fini::decompress("");});rejects([]{fini::decompress("invalid");});
    auto zipped=fini::compress(std::string(100000,'a'));check(fini::decompress(zipped)==std::string(100000,'a'),"High compression ratio");
    rejects([&]{fini::decompress(zipped.substr(0,zipped.size()-1));});
    check(state.request("packPosition",{-1,42})=="/////yoAAAA=","Win32 position vector");
    check(state.request("unpackPosition",{"/////yoAAAA="})==Json::array({-1,42}),"Win32 position decode");rejects([&]{state.request("unpackPosition",{"AA=="});});
    Json values={{"flags",0x80000001u},{"values",Json::array({{{"type",0},{"value",-42}},{{"type",2},{"value",15.25}}})},{"strings",Json::array({"你好 😀",""})}};
    auto encoded=fini::packValues(values);check(fini::unpackValues(encoded,2,2)==values,"Win32 CValue/UTF16 object state");
    auto bytes=fini::unbase64(encoded);check(bytes.size()==50,"Win32 4-byte flags,16-byte values,UTF16LE");check(static_cast<unsigned char>(bytes[0])==1&&static_cast<unsigned char>(bytes[3])==128,"Flags LE");
    rejects([&]{fini::unpackValues(encoded,3,2);});rejects([&]{fini::unpackValues(encoded.substr(4),2,2);});
    state.localizeFrom("[zh]\n中文 😀 = 本地文本\n");state.language="zh";check(state.get("mixed","value")=="本地文本","Localization");state.language="missing";check(state.get("mixed","value")=="中文 😀","Localization fallback");
    std::string plain="[数据]\r\n值=你好 😀\r\n",key="密码/😀/long-key";
    check(fini::crypt(fini::crypt(plain,key,true),key,false)==plain,"Shared encryption Unicode password");
    // Independent existing asset fixture verifies exact historical ciphertext reuse.
    auto root=std::filesystem::path(__FILE__).parent_path()/"../../..";
    auto asset=root/"NeoPic/ToInstall/Files/Examples/NeoPic";
    auto encrypted=asset/"Test_Encrypt.dat",jpeg=asset/"Test_Origin.jpg";
    if(std::filesystem::exists(encrypted)&&std::filesystem::exists(jpeg)){
        auto cipher=fini::read(encrypted.string(),"");auto clear=fini::read(jpeg.string(),"");
        check(fini::crypt(cipher,"12345678ABCDEFGH",false)==clear,"Existing Windows encrypted asset");check(fini::crypt(clear,"12345678ABCDEFGH",true)==cipher,"Exact Windows ciphertext");
    }else throw std::runtime_error("Missing existing asset fixture");
    auto path=std::filesystem::temp_directory_path()/"fini-port-test.ini";
    struct Cleanup{std::filesystem::path p;~Cleanup(){std::filesystem::remove(p);}} cleanup{path};
    fini::State file;file.request("autoOn",{path.string(),key});file.request("set",{"A","B","first"});file.load("[Other]\nitem=value\n");
    fini::State saved;saved.loadFile(path.string(),key);check(saved.get("A","B")=="first","Auto save before replacement");
    saved.request("set",{"A","B","updated"});saved.loadFile(path.string(),key);check(saved.get("A","B")=="updated","Auto save before reading same file");
    auto before=saved.save();rejects([&]{saved.loadFile(path.string()+".missing",key);});check(saved.save()==before,"Failed input preserves state");
    saved.request("set",{"A","B","second"});saved.release();saved.flush();check(fini::read(path.string(),key).find("updated")!=std::string::npos,"Release suppresses auto save");
    std::cout<<"PASS: shared INI, codecs, Win32 formats, existing ciphertext, lifecycle and errors\n";
}
