#include "Split.h"

Split::Split() {
    this->Reserve();
}

Split::~Split() {
    this->ResetSplit();
}

void Split::ResetSplit() {
    //reset unicode
    this->Unicode = true;

    //release str
    this->ReleaseSplitSrcStr();
    this->SplitDataStr.clear();

    //release vec
    this->SplitStrVec.clear();
    this->SubStringVec.clear();
    this->KeyWordPairVec.clear();

    //release strs
    this->MatchedStr.clear();
    this->ReplacedStr.clear();

    //reset flag
    this->Init = false;
    this->RemoveEmptyLine = false;
    this->RemoveCommnet = false;
    this->RemoveIndent = false;
    this->KeyWord = false;

    this->Flag = this->DefaultFlag;
}

bool Split::LoadFile(const wchar_t* FilePath, const wchar_t* Key, bool Unicode) {
    this->SetUnicode(Unicode);
    const auto bRet = this->OpenFile(FilePath);

    if(!bRet) {
        return false;
    }

    if (wcscmp(Key, L"") != 0) {
        this->GenerateKey(Key);
        this->Decrypt();
        this->LoadData(this->GetOutputStr());
    }
    else {
        this->LoadData(this->GetInputStr());
    }

    return true;
}
bool Split::LoadFile(const std::wstring& FilePath, const std::wstring& Key, bool Unicode) {
    return LoadFile(FilePath.c_str(), Key.c_str());
}

void Split::LoadData() {
    this->LoadData(this->GetInputStr(), this->GetInputStrLength());
    this->ReleaseInputStr();
}

void Split::LoadData(const char* Src) {
    this->LoadData(Src, strlen(Src));
}

void Split::LoadData(const char* Src, size_t Len) {
    if (Src == nullptr) {
        return;
    }

    this->ReleaseSplitSrcStr();

    //BOM
    if ((Len >= 3) && (memcmp(Src, UTF8_SIGNATURE, 3) == 0)) {
        Src += 3;
        Len -= 3;
        this->Unicode = true;
    }

    if (Len == 0) {
        return;
    }

    UINT CodePage = this->Unicode ? CP_UTF8 : CP_ACP;

    size_t OutputLenth = this->GetSize(Src, Len, CodePage);
    if (OutputLenth == (size_t)(-1)) {
        return;
    }

    NewSplitSrc(OutputLenth);

    if (!this->Convert(Src, Len, this->SplitSrcStr, OutputLenth, CodePage)) {
        return;
    }
	
    this->Reserve(this->SplitSrcStrLength / RESERVE_MAGNUM);
}

void Split::LoadData(const std::wstring& Src) {
    this->LoadData(Src.c_str(), Src.length());
}

void Split::LoadData(const wchar_t* Src) {
    this->LoadData(Src, wcslen(Src));
}

void Split::LoadData(const wchar_t* Src, size_t Len) {
    this->ReleaseSplitSrcStr();
    NewSplitSrc(Len);
    memcpy(this->SplitSrcStr, Src, sizeof(wchar_t) * Len);
}

void Split::ReSetRegexFlag() {
    this->Flag = this->DefaultFlag;
}

void Split::SetRegexFlag(RegexFlag Flag) {
    this->Flag = Flag;
}
void Split::SetCaseInsensitive(bool Enable) {
    this->Flag = Enable ? this->Flag | icase : this->Flag ^ icase;
}

void Split::InitSplit(const wchar_t* Split) {
    this->LineReg.assign(Split, this->Flag);
    this->LineRegStr = Split;
    this->Init = true;
}

void Split::InitEmptyLine(const wchar_t* EnptyLine) {
    this->EmptyLineReg.assign(EnptyLine, this->Flag);
    this->RemoveEmptyLine = true;
}

void Split::InitComment(const wchar_t* Comment) {
    this->CommentReg.assign(Comment, this->Flag);
    this->RemoveCommnet = true;
}

void Split::InitIndent(const wchar_t* Indent) {
    this->IndentReg.assign(Indent, this->Flag);
    this->RemoveIndent = true;
}

void Split::InitKeyWord(const wchar_t* KeyWord) {
    this->KeyWordReg.assign(KeyWord, this->Flag);
    this->KeyWord = true;
}

void Split::InitRegex(const wchar_t* Split, const wchar_t* EnptyLine, const wchar_t* Comment, const wchar_t* Indent, const wchar_t* KeyWord) {
    if (wcslen(Split) != 0) {
        this->InitSplit(Split);
    }
    if (wcslen(EnptyLine) != 0) {
        this->InitEmptyLine(EnptyLine);
    }
    if (wcslen(Comment) != 0) {
        this->InitComment(Comment);
    }
    if (wcslen(Indent) != 0) {
        this->InitIndent(Indent);
    }
    if (wcslen(KeyWord) != 0) {
        this->InitKeyWord(KeyWord);
    }
}

void Split::SplitData() {
    //Reset
    this->SplitStrVec.clear();
    this->KeyWordPairVec.clear();

    //remove comment
    this->SplitDataStr = this->RemoveCommnet 
        ? regex_replace(std::wstring(this->SplitSrcStr), this->CommentReg, L"") 
        : this->SplitSrcStr;
    
    //merge multiple
    //wregex Merge(regex_replace(L"(REPLACE){2,}", wregex(L"REPLACE"), this->LineRegStr));
    //this->SplitScrStrNoComment = regex_replace(this->SplitScrStrNoComment, Merge, this->LineRegStr);

    //iterate lines	
    if (!this->Init) {
        return;
    }

    wsregex_token_iterator pos(this->SplitDataStr.begin(), this->SplitDataStr.end(), this->LineReg, -1);
    wsregex_token_iterator end;

    std::wstring result;
    std::wstring Tmp;
	
    for (; pos != end; pos++) {
        //current str
        //std::wstring Tmp = pos->str();
        Tmp = pos->str();
		
        //remove empty line
        if (this->RemoveEmptyLine && regex_match(Tmp, this->EmptyLineReg)) {
            continue;
        }

        //remove indent
        this->SplitStrVec.emplace_back(this->RemoveIndent ? regex_replace(Tmp, this->IndentReg, L"") : Tmp);

        //generate result
        //result.append(this->SplitStrVec.back());
        //result.append(this->LineRegStr);

        //update keyword
        if (this->KeyWord && regex_match(this->SplitStrVec.back(), this->KeyWordReg)) {
            this->KeyWordPairVec.emplace_back(this->SplitStrVec.size() - 1, this->SplitStrVec.back());
        }
    }

    //this->SplitDataStr = result.substr(0, result.length() - this->LineRegStr.length());

    return;
}

int Split::GetSubStringPos(const wchar_t* SubStr, size_t Sub) {
    return this->GetSubStringPos(this->GetSplitData(), SubStr, Sub);
}

int Split::GetSubStringPos(const wchar_t* Src, const wchar_t* SubStr, size_t Sub) {
    return this->GetSubStringPos(std::wstring(Src), SubStr, Sub);
}

int Split::GetSubStringPos(const std::wstring& Src, const wchar_t* SubStr, size_t Sub, bool SaveAll) {
    this->MatchedStr.clear();

    wregex SubString(SubStr, this->Flag);
    wsmatch MatchedStr;

    std::wstring::const_iterator StrBegin = Src.begin();
    std::wstring::const_iterator StrEnd = Src.end();

    size_t CurPos = 0;
    size_t PrePos = 0;
    for (size_t i = 0; regex_search(StrBegin, StrEnd, MatchedStr, SubString) && (i <= Sub); i++, PrePos = Src.length() - MatchedStr.suffix().length(), StrBegin = MatchedStr[0].second) {
        CurPos = PrePos + MatchedStr.position((size_t)0);

        if (SaveAll) {
            this->SubStringVec.emplace_back(MatchedStr[0]);
        }
        if (Sub == -1) {
            this->MatchedStr = MatchedStr[0];
        }
        if (i == Sub) {
            this->MatchedStr = MatchedStr[0];
            return CurPos;
        }
    }

    return Sub == -1 ? CurPos : -1;
}

void Split::GetAllSubString(const wchar_t* SubStr) {
    this->GetAllSubString(this->GetSplitData(), SubStr);
}

void Split::GetAllSubString(const wchar_t* Src, const wchar_t* SubStr) {
    this->GetAllSubString(std::wstring(Src), SubStr);
}

void Split::GetAllSubString(const std::wstring& Src, const wchar_t* SubStr) {
    this->SubStringVec.clear();
    this->GetSubStringPos(Src, SubStr, -1, true);;
}

const wchar_t* Split::GetNextKeyWord(size_t StartPos) {
    //match all in this case
    return this->GetNextKeyWord(StartPos, ALL);
}

const wchar_t* Split::GetNextKeyWord(size_t StartPos, const wchar_t* KeyWord) {
    //valid current pos    
    if (!((this->KeyWordPairVec.size() > 0) && (StartPos < this->KeyWordPairVec.back().first) && (StartPos >= 0))) {
        return nullptr;
    }

    wregex KW(KeyWord, this->Flag);

    for (auto& it : this->KeyWordPairVec) {
        if ((it.first > StartPos) && regex_match(it.second.c_str(), KW))
            return it.second.c_str();
    }

    return nullptr;
}

int Split::GetNextKeyWordPos(size_t StartPos) {
    //match all in this case
    return this->GetNextKeyWordPos(StartPos, ALL);
}

int Split::GetNextKeyWordPos(size_t StartPos, const wchar_t* KeyWord) {
    //valid current pos
    if (!((this->KeyWordPairVec.size() > 0) &&(StartPos < this->KeyWordPairVec.back().first) && (StartPos >= 0))) {
        return -1;
    }

    wregex KW(KeyWord, this->Flag);

    for (auto& it : this->KeyWordPairVec) {
        if ((it.first > StartPos) && regex_match(it.second.c_str(), KW))
            return it.first;
    }

    return -1;
}
