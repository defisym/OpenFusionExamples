#include "Split.h"

size_t Split::GetSize(const char* Src, size_t Len) {
    int Size = MultiByteToWideChar(CP_UTF8, 0, Src, (int)Len, 0, 0);

    return (size_t)((Size > 0) ? Size : -1);
}

bool Split::Convert(const char* Src, size_t SrcLen, const wchar_t* Des, size_t DesLen) {
    int Size = MultiByteToWideChar(CP_UTF8, 0, Src, (int)SrcLen, (wchar_t*)Des, (int)DesLen);

    return (Size > 0);
}

Split::Split() {

}

Split::~Split() {
    this->ResetSplit();
}

void Split::ResetSplit() {
    //release str
    this->ReleaseSplitSrcStr();

    //release vec
    this->SplitStrVec.clear();

    //release map
    this->KeyWordMap.clear();

    //release strs
    this->MatchedStr.clear();
    this->ReplacedStr.clear();

    //reset flag
    this->RemoveEmptyLine = false;
    this->RemoveCommnet = false;
    this->RemoveIndent = false;
    this->KeyWord = false;
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
    }

    if (Len == 0) {
        return;
    }

    size_t OutputLenth = this->GetSize(Src, Len);
    if (OutputLenth == (size_t)(-1)) {
        return;
    }

    this->SplitSrcStrLength = OutputLenth + 1;
    this->SplitSrcStr = new wchar_t[this->SplitSrcStrLength];
    memset(this->SplitSrcStr, 0, sizeof(wchar_t) * (this->SplitSrcStrLength));

    if (!this->Convert(Src, Len, this->SplitSrcStr, OutputLenth)) {
        return;
    }
}

void Split::LoadData(const std::wstring& Src) {
    this->LoadData(Src.c_str(), Src.length());
}

void Split::LoadData(const wchar_t* Src) {
    this->LoadData(Src, wcslen(Src));
}

void Split::LoadData(const wchar_t* Src, size_t Len) {
    this->SplitSrcStrLength = Len + 1;
    this->SplitSrcStr = new wchar_t[this->SplitSrcStrLength];
    memset(this->SplitSrcStr, 0, sizeof(wchar_t) * (this->SplitSrcStrLength));
    memcpy(this->SplitSrcStr, Src, sizeof(wchar_t) * Len);
}

void Split::ReSetRegexFlag() {
    this->Flag = this->DefaultFlag;
}

void Split::SetRegexFlag(RegexFlag Flag) {
    this->Flag = Flag;
}
void Split::SetCaseInsensitive(bool Enable) {
    this->Flag = Enable ? this->Flag | std::regex_constants::icase : this->Flag ^ std::regex_constants::icase;
}

void Split::InitSplit(const wchar_t* Split) {
    this->LineReg.assign(Split, this->Flag);
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
    //remove comment
    this->SplitScrStrNoComment = this->RemoveCommnet ? std::regex_replace(this->SplitSrcStr, this->CommentReg, L"") : this->SplitSrcStr;

    //lterate lines
    std::wsregex_token_iterator pos(this->SplitScrStrNoComment.begin(), this->SplitScrStrNoComment.end(), this->LineReg, -1);
    std::wsregex_token_iterator end;

    for (; pos != end; pos++) {
        //current str
        std::wstring Tmp = pos->str();

        //remove empty line
        if (this->RemoveEmptyLine && std::regex_match(Tmp, this->EmptyLineReg)) {
            continue;
        }

        //remove indent
        this->InsertItem(this->RemoveIndent ? std::regex_replace(Tmp, this->IndentReg, L"") : Tmp);

        //update keyword
        if (this->KeyWord && std::regex_match(this->SplitStrVec.back(), this->KeyWordReg)) {
            this->KeyWordMap.insert(std::map<size_t, std::wstring>::value_type(this->SplitStrVec.size() - 1, this->SplitStrVec.back()));
        }
    }
}

int Split::GetSubStringPos(const wchar_t* SubStr, size_t Sub) {
    return this->GetSubStringPos(this->SplitScrStrNoComment, SubStr, Sub);
}

int Split::GetSubStringPos(const wchar_t* Src, const wchar_t* SubStr, size_t Sub) {
    return this->GetSubStringPos((std::wstring)Src, SubStr, Sub);
}

int Split::GetSubStringPos(const std::wstring& Src, const wchar_t* SubStr, size_t Sub, bool SaveAll) {
    this->MatchedStr.clear();

    std::wregex SubString(SubStr, this->Flag);
    std::wsmatch MatchedStr;

    std::wstring::const_iterator StrBegin = Src.begin();
    std::wstring::const_iterator StrEnd = Src.end();

    size_t CurPos = 0;
    size_t PrePos = 0;
    for (size_t i = 0; std::regex_search(StrBegin, StrEnd, MatchedStr, SubString) && (i <= Sub); i++, PrePos = Src.length() - MatchedStr.suffix().length(), StrBegin = MatchedStr[0].second) {
        CurPos = PrePos + MatchedStr.position(0);
        if (SaveAll) {
            this->SubStringVec.emplace_back(MatchedStr[0]);
        }
        if (i == Sub) {
            this->MatchedStr = MatchedStr[0];
            return CurPos;
        }
    }

    return Sub == -1 ? CurPos : -1;
}

void Split::GetAllSubString(const wchar_t* SubStr) {
    this->GetAllSubString(this->SplitScrStrNoComment, SubStr);
}

void Split::GetAllSubString(const wchar_t* Src, const wchar_t* SubStr) {
    this->GetAllSubString((std::wstring)Src, SubStr);
}

void Split::GetAllSubString(const std::wstring& Src, const wchar_t* SubStr) {
    this->SubStringVec.clear();
    this->GetSubStringPos(Src, SubStr, -1, true);;
}

int Split::GetNextKeyWordPos(size_t StartPos) {
    //valid current pos
    if (!((StartPos < this->SplitStrVec.size()) && (StartPos >= 0))) {
        return -1;
    }

    for (auto& it : this->KeyWordMap) {
        if (it.first > StartPos)
            return it.first;
    }

    return -1;
}

int Split::GetNextKeyWordPos(size_t StartPos, const wchar_t* KeyWord) {
    //valid current pos
    if (!((StartPos < this->SplitStrVec.size()) && (StartPos >= 0))) {
        return -1;
    }

    std::wregex KW(KeyWord, this->Flag);

    for (auto& it : this->KeyWordMap) {
        if ((it.first > StartPos) && std::regex_match(it.second.c_str(), KW))
            return it.first;
    }

    return -1;
}


