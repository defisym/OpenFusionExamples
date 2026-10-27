#include "GeneralDefinition.h"
#include "StrNum.h"
#include <iostream>
#include <clocale>
#include <stdexcept>

std::string helperOtherTU();
static int checks;
static void check(bool value, const char* message) {
    ++checks;
    if (!value) { throw std::runtime_error(message); }
}
static void conversionChecks() {
    const std::wstring text = L"A中文\U0001F680";
    const std::string utf8 = "A\xE4\xB8\xAD\xE6\x96\x87\xF0\x9F\x9A\x80";
    check(to_wide_string(utf8) == text, "UTF8 decode");
    check(to_byte_string(text) == utf8, "UTF8 encode");
    const std::string gbk = "\xD6\xD0\xCE\xC4";
    check(to_wide_string(gbk, 936) == L"中文", "CP936 decode");
    check(to_byte_string(L"中文", 936) == gbk, "CP936 encode");
    check(to_wide_string(std::string("\x80",1),1252) == L"\u20AC", "CP1252 decode");
    check(to_byte_string(L"\u20AC",1252) == std::string("\x80",1), "CP1252 encode");
    check(to_byte_string(L"é",28591) == std::string("\xE9",1), "ISO8859-1 mapping");
#ifndef _WIN32
    check(to_wide_string(std::string("\x41\0\x2D\x4E",4),1200) == L"A中", "UTF16LE decode");
    check(to_byte_string(L"A中",1201) == std::string("\0\x41\x4E\x2D",4), "UTF16BE encode");
#endif
    check(to_wide_string(std::string("A\0B",3)) == std::wstring(L"A\0B",3), "embedded NUL decode");
    check(to_byte_string(std::wstring(L"A\0B",3)) == std::string("A\0B",3), "embedded NUL encode");
    std::wstring longWide(4000,L'中');
    auto longBytes = to_byte_string(longWide);
    check(longBytes.size() == 12000 && to_wide_string(longBytes) == longWide, "E2BIG loop");
    std::wstring wide = L"unchanged";
    std::string bytes = "unchanged";
#ifndef _WIN32
    check(!to_wide_string(wide,"\xC0\xAF",2,CP_UTF8) && wide == L"unchanged", "invalid UTF8 preserves output");
    check(!to_wide_string(wide,"\xE4\xB8",2,CP_UTF8) && wide == L"unchanged", "incomplete UTF8");
    check(!to_wide_string(wide,"x",1,999999), "unsupported codepage");
    check(!to_wide_string(wide,nullptr,1,CP_UTF8), "null byte input");
    check(!to_wide_string(wide,"",0,CP_UTF8), "zero input matches WinAPI failure contract");
    check(!to_byte_string(bytes,L"中",1,20127) && bytes == "unchanged", "no silent ASCII substitution");
    const wchar_t invalid[] = {static_cast<wchar_t>(0xD800)};
    check(!to_byte_string(bytes,invalid,1,CP_UTF8) && bytes == "unchanged", "invalid wide input");
    check(!to_byte_string(bytes,L"x",(std::numeric_limits<size_t>::max)(),CP_UTF8), "wide length overflow");
#endif
    bool unicode = false;
    const std::string bom = "\xEF\xBB\xBF" + utf8;
    check(LoadData(wide,bom.data(),bom.size(),unicode) && unicode && wide == text, "BOM loading");
    check(!LoadData(wide,"\xEF\xBB\xBF",3,unicode), "BOM-only document");
    check(SaveData(bytes,text.data(),text.size(),true) && bytes == utf8, "SaveData Unicode");
#ifndef _WIN32
    char input[] = "x";
    char* cursor = input;
    size_t left = 1;
    char target[1]; char* targetCursor = target; size_t targetLeft = 1;
    auto constInput = [](iconv_t, const char** source, size_t* remaining, char**, size_t*) -> size_t {
        ++*source; --*remaining; return 0;
    };
    check(DeLibDetail::RunIconv(constInput, (iconv_t)0, &cursor, &left, &targetCursor, &targetLeft)==0
            && cursor==input+1 && left==0, "const-input iconv signature adapter");
    std::string stateful;
    check(DeLibDetail::ConvertBytes(stateful,"\xE6\x97\xA5",3,"UTF-8","ISO-2022-JP"), "stateful encoder");
    check(stateful.size() >= 3 && stateful.substr(stateful.size()-3) == "\x1B(B", "stateful flush");
    check(to_wide_string(to_byte_string(text,CP_ACP),CP_ACP) == text, "default ACP UTF8 compatibility");
    unicode = false;
    check(LoadData(wide,utf8.data(),utf8.size(),unicode) && !unicode && wide==text, "default ACP LoadData");
    check(SaveData(bytes,text.data(),text.size(),false) && bytes==utf8, "default ACP SaveData");
#endif
}
static void stringChecks() {
    wchar_t* owned = nullptr;
    NewStr(owned,L"ab中"); check(std::wstring(owned) == L"ab中", "NewStr allocation");
    NewStr(owned,owned+1); check(std::wstring(owned) == L"b中", "NewStr self-alias");
    NewStr(owned,std::wstring(L"")); check(owned[0] == 0, "NewStr empty"); delete[] owned;
    std::wstring copied; NewStr(copied,std::wstring(L"copy")); check(copied==L"copy", "NewStr string overload");
    char narrow[] = "  ab  "; wchar_t wide[] = L"  AB  ";
    check(GetTrimmedStr(narrow,6) == "ab", "narrow trim");
    check(GetTrimmedStr(wide,6) == L"AB", "wide trim");
    check(GetTrimmedStr(narrow,0).empty(), "empty narrow view");
    check(GetTrimmedStr(static_cast<wchar_t*>(nullptr),0).empty(), "empty null view");
    char spaces[] = "   "; check(GetTrimmedStr(spaces,3).empty(), "all spaces view");
    std::wstring_view view = L"  x "; check(GetTrimmedStr(view) == L"x", "wide view overload");
    std::string_view byteView = " x "; check(GetTrimmedStr(byteView) == "x", "byte view overload");
    std::wstring trim; TrimStr(trim); check(trim.empty(), "empty owned trim");
    trim = L"    "; TrimStr(trim); check(trim.empty(), "all spaces owned trim");
    trim = L"--x++"; TrimStr(trim,{L'-'},{L'+'}); check(trim==L"x", "custom owned trim");
    std::string lower = "ABc"; ToLower(lower); check(lower == "abc", "lower mutable bytes");
    std::wstring upper = L"abC"; ToUpper(upper); check(upper == L"ABC", "upper mutable wide");
    const std::string source = "aBc"; check(ToUpper(source) == "ABC" && source == "aBc", "upper const copy");
    const std::wstring wideSource = L"AbC"; check(ToLower(wideSource)==L"abc", "lower const copy");
    char buffer[] = "ab"; ToUpper(buffer,2); check(std::string(buffer)=="AB", "upper pointer overload");
    wchar_t wbuffer[] = L"AB"; ToLower(wbuffer,2); check(std::wstring(wbuffer)==L"ab", "lower pointer overload");
    check(CharIEqu('A','a') && CharIEqu(L'B',L'b'), "case character equality");
    check(StrEqu("a","a") && StrEqu(L"中",L"中"), "exact string equality");
    check(StrIEqu("aBc","AbC") && StrIEqu(L"aBc",L"AbC"), "insensitive equality");
    check(!StrIEqu("a","ab") && !StrIEqu(L"a",L"b"), "insensitive mismatches");
    check(StrEmpty("") && StrEmpty(L"") && !StrEmpty("a"), "empty predicates");
    check(StringViewEqu(std::wstring_view(L"ab"),L"ab"), "view equality");
    check(StringViewIEqu(std::wstring_view(L"Ab"),L"aB"), "view-pointer insensitive");
    check(StringViewIEqu(std::wstring_view(L"Ab"),std::wstring_view(L"aB")), "view-view insensitive");
    check(StringAppend(L"ab",L"abc") && !StringAppend(L"abc",L"ab"), "prefix predicate");
    check(StringIAppend(L"AB",L"abc"), "case-insensitive prefix predicate");
    check(StringViewToString(std::wstring_view(L"a\0b",3)) == std::wstring(L"a\0b",3), "view copy with NUL");
    check(SplitString(std::string(",a,,b,"),',') == std::vector<std::string>({"a","b"}), "narrow split");
    check(SplitString(std::wstring(L"|a||b|"),L'|') == std::vector<std::wstring>({L"a",L"b"}), "wide split");
    check(SplitString(std::string(""),',').empty(), "empty split");
    int calls=0; SplitStringUntilCore<char>("a,b,c",',',[&](auto){return ++calls < 2;});
    check(calls==2, "split early exit");
    auto numbers = SplitString<int,char>(std::string("1,2"),',',[](auto s){return s[0]-'0';});
    check(numbers==std::vector<int>({1,2}), "mapped split");
    // Locale-sensitive wide conversion uses wide functions, never narrow ctype on a Unicode code point.
    if (std::setlocale(LC_CTYPE,"C.UTF-8")) {
        std::wstring word=L"äé"; ToUpper(word); check(word==L"ÄÉ", "wide locale case conversion");
        check(StrIEqu(L"Ä",L"ä"), "wide locale equality");
        std::setlocale(LC_CTYPE,"C");
    }
}
static void helperChecks() {
    check(HASHER_MOVE(5) == HASHER_MAGICNUMBER + (size_t{5}<<6) + (size_t{5}>>2), "original hash helper");
    check(Range<size_t>(9,0,3)==3 && Range(-1,0,3)==0, "range helper");
    int value=10; UpdateRange(value,0,5); check(value==5, "range update");
    check(NearlyEqualFLT(1,1) && NearlyEqualDBL(1,1) && NearlyEqualLDBL(1,1), "float helpers");
    check(!NearlyEqualDBL(1,2), "unequal float helper");
    std::vector<int> sorted{1,3}; InsertSortedUpperBound(sorted,2); InsertSortedLowerBound(sorted,2);
    check(sorted==std::vector<int>({1,2,2,3}), "sorted inserts");
    std::vector<int> descending{3,1}; auto compare=[](int a,int b){return a>b;};
    InsertSortedUpperBound(descending,2,compare); InsertSortedLowerBound(descending,2,compare);
    check(descending==std::vector<int>({3,2,2,1}), "custom sorted inserts");
    check(_itos(size_t{123})==L"123" && _itos<std::string>(-12)=="-12", "number helper reuse");
    check(_stoi(std::wstring(L"123"))==123, "number parser");
    int CONNECT_STR(token,Name)=5; check(tokenName==5 && std::string(STRINGIFY(test))=="test", "macro helpers");
#ifndef _WIN32
    auto folder = CUR_FOLDER;
    auto asset = RELATIVE_PATH("asset.txt");
    auto normalized = RELATIVE_PATH("sub/../asset.txt");
    auto absolute = RELATIVE_PATH("/tmp/delib-absolute-asset");
    auto unicode = RELATIVE_PATH("空 格.txt");
    check(std::string(folder).find("GeneralDefinitionTest.cpp")==std::string::npos, "source directory macro");
    check(std::string(asset)==std::string(folder)+"asset.txt", "source relative path macro");
    check(std::string(normalized)==std::string(folder)+"asset.txt", "lexical dot-dot normalization");
    check(std::string(absolute)=="/tmp/delib-absolute-asset", "absolute path suffix");
    check(std::string(unicode)==std::string(folder)+"空 格.txt", "path with Unicode and spaces");
    auto owned = RELATIVE_PATH("stable.txt");
    const char* stable = owned;
    auto other = RELATIVE_PATH("other.txt");
    check(std::string(stable)==std::string(folder)+"stable.txt", "pointer stays valid while owner lives");
    auto copied = owned;
    char* writable = copied;
    writable[0] = writable[0]=='X' ? 'Y' : 'X';
    check(std::string(owned)==std::string(stable) && std::string(copied)!=std::string(owned), "copy owns an independent writable buffer");
    const auto constOwner = owned;
    const char* readonly = constOwner;
    check(std::string(readonly)==std::string(owned), "const owner pointer conversion");
    check(owned.size()==std::string(stable).size(), "writable buffer length");
    using Owner = decltype(owned);
    static_assert(std::is_convertible_v<Owner&, char*>);
    static_assert(std::is_convertible_v<const Owner&, const char*>);
    static_assert(!std::is_convertible_v<const Owner&, char*>);
    static_assert(!std::is_convertible_v<Owner, const char*>);
    static_assert(!std::is_convertible_v<Owner, char*>);

#endif
}
int main() {
    conversionChecks(); stringChecks(); helperChecks();
    check(helperOtherTU() == to_byte_string(L"跨单元"), "header-only multi-TU linkage");
    std::cout << "PASS: " << checks << " GeneralDefinition helper checks\n";
}
