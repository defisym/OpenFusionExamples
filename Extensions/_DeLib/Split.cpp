#include "Split.h"

void Split::ResetSplit() {
    //reset unicode
    this->Unicode = true;

    //release str
    this->SplitSrcStr.clear();
    this->SplitDataStr.clear();

    //release vec
    this->SplitStrVec.clear();
    this->SubStringVec.clear();
    this->KeyWordPairVec.clear();

    //release strs
    this->MatchedStr.clear();
    this->ReplacedStr.clear();

    //reset flag
    this->bLineReg = false;
    this->RemoveEmptyLine = false;
    this->RemoveComment = false;
    this->RemoveIndent = false;
    this->KeyWord = false;

    this->Flag = this->DefaultFlag;
}

bool Split::LoadFile(const wchar_t* pFilePath, const wchar_t* pKey, bool bUnicode) {
    this->SetUnicode(bUnicode);

    Encryption encrypt;

    if (!encrypt.OpenFile(pFilePath)) {
        return false;
    }

    const char* pData = nullptr;

    if (wcscmp(pKey, L"") != 0) {
        encrypt.GenerateKey(pKey);
        encrypt.Decrypt();
        pData = encrypt.GetOutputStr();
    }
    else {
        pData = encrypt.GetInputStr();
    }

    return this->LoadData(pData);
}
bool Split::LoadFile(const std::wstring& FilePath, const std::wstring& Key, bool bUnicode) {
    return LoadFile(FilePath.c_str(), Key.c_str(), bUnicode);
}

// multibyte, do conversion
bool Split::LoadData(const std::string& Src) {
    return this->LoadData(Src.c_str(), Src.length());
}
bool Split::LoadData(const char* Src) {
    return this->LoadData(Src, strlen(Src));
}
bool Split::LoadData(const char* Src, size_t Len) {
    if (!LoadData(this->SplitSrcStr, Src, Len, this->Unicode)) {
        return false;
    }

    this->Reserve(this->SplitSrcStr.length() / RESERVE_MAGNUM);

    return true;
}

// wide char, load directly
bool Split::LoadData(const std::wstring& Src) {
    return this->LoadData(Src.c_str(), Src.length());
}
bool Split::LoadData(const wchar_t* Src) {
    return this->LoadData(Src, wcslen(Src));
}
bool Split::LoadData(const wchar_t* Src, size_t Len) {
    this->SplitSrcStr = std::wstring(Src, Len);

    return true;
}

void Split::ResetRegexFlag() {
    this->Flag = this->DefaultFlag;
}

void Split::SetRegexFlag(const RegexFlag Flag) {
    this->Flag = Flag;
}
void Split::SetCaseInsensitive(bool Enable) {
    this->Flag = Enable ? this->Flag | icase : this->Flag ^ icase;
}

void Split::InitSplit(const wchar_t* Split) {
    this->LineReg.assign(Split, this->Flag);
    this->LineRegStr = Split;
    this->bLineReg = true;
}

void Split::InitEmptyLine(const wchar_t* EmptyLine) {
    this->EmptyLineReg.assign(EmptyLine, this->Flag);
    this->EmptyLineRegStr = EmptyLine;
    this->RemoveEmptyLine = true;
}

void Split::InitComment(const wchar_t* Comment) {
    this->CommentReg.assign(Comment, this->Flag);
    this->CommentRegStr = Comment;
    this->RemoveComment = true;
}

void Split::InitIndent(const wchar_t* Indent) {
    this->IndentReg.assign(Indent, this->Flag);
    this->IndentRegStr = Indent;
    this->RemoveIndent = true;
}

void Split::InitKeyWord(const wchar_t* pKeyWord) {
    this->KeyWordReg.assign(pKeyWord, this->Flag);
    this->KeyWordRegStr = pKeyWord;
    this->KeyWord = true;
}

void Split::InitRegex(const wchar_t* Split,
    const wchar_t* EmptyLine,
    const wchar_t* Comment,
    const wchar_t* Indent,
    const wchar_t* pKeyWord) {
    if (wcslen(Split) != 0) {
        this->InitSplit(Split);
    }
    if (wcslen(EmptyLine) != 0) {
        this->InitEmptyLine(EmptyLine);
    }
    if (wcslen(Comment) != 0) {
        this->InitComment(Comment);
    }
    if (wcslen(Indent) != 0) {
        this->InitIndent(Indent);
    }
    if (wcslen(pKeyWord) != 0) {
        this->InitKeyWord(pKeyWord);
    }
}

void Split::SplitData() {
    //Reset
    this->SplitStrVec.clear();
    this->KeyWordPairVec.clear();

    //remove comment
    this->SplitDataStr = this->RemoveComment
        ? regex_replace(this->SplitSrcStr, this->CommentReg, L"")
        : this->SplitSrcStr;

    //iterate lines	
    if (!this->bLineReg) {
        return;
    }

    wsregex_token_iterator pos(this->SplitDataStr.begin(), this->SplitDataStr.end(), this->LineReg, -1);

    for (const wsregex_token_iterator end; pos != end; ++pos) {
        //current str
        std::wstring Tmp = pos->str();

        //remove empty line
        if (this->RemoveEmptyLine && regex_match(Tmp, this->EmptyLineReg)) {
            continue;
        }

        //remove indent
        this->SplitStrVec.emplace_back(this->RemoveIndent ? regex_replace(Tmp, this->IndentReg, L"") : Tmp);

        //update keyword
        if (this->KeyWord && regex_match(this->SplitStrVec.back(), this->KeyWordReg)) {
            this->KeyWordPairVec.emplace_back(this->SplitStrVec.size() - 1, this->SplitStrVec.back());
        }
    }
}

size_t Split::GetHash() const {
    size_t seed = 1 + 1 + 1 + 5;

    const auto RegexFlag = static_cast<int>(Flag);
    seed ^= RegexFlag + HASHER_MOVE(seed);

    const auto SplitFlag = (this->Unicode >> 5)
        | (this->bLineReg >> 4)
        | (this->RemoveEmptyLine >> 3)
        | (this->RemoveComment >> 2)
        | (this->RemoveIndent >> 1)
        | (this->KeyWord >> 0);
    seed ^= SplitFlag + HASHER_MOVE(seed);

    seed ^= std::hash<std::wstring>{}(this->SplitSrcStr) + HASHER_MOVE(seed);

    seed ^= std::hash<std::wstring>{}(this->LineRegStr) + HASHER_MOVE(seed);
    seed ^= std::hash<std::wstring>{}(this->EmptyLineRegStr) + HASHER_MOVE(seed);
    seed ^= std::hash<std::wstring>{}(this->CommentRegStr) + HASHER_MOVE(seed);
    seed ^= std::hash<std::wstring>{}(this->IndentRegStr) + HASHER_MOVE(seed);
    seed ^= std::hash<std::wstring>{}(this->KeyWordRegStr) + HASHER_MOVE(seed);

    return seed;
}

void Split::GetResult(SplitResult* pSplitResult) const {
    if (pSplitResult != nullptr) {
        pSplitResult->SplitDataStr = this->SplitDataStr;
        pSplitResult->SplitStrVec = this->SplitStrVec;
        pSplitResult->KeyWordPairVec = this->KeyWordPairVec;
    }
}

void Split::SetResult(const SplitResult* pSplitResult) {
    if (pSplitResult != nullptr) {
        this->SplitDataStr = pSplitResult->SplitDataStr;
        this->SplitStrVec = pSplitResult->SplitStrVec;
        this->KeyWordPairVec = pSplitResult->KeyWordPairVec;
    }
}

int Split::GetSubStringPos(const wchar_t* SubStr, size_t Sub) {
    return this->GetSubStringPos(this->GetSplitData(), SubStr, Sub);
}

int Split::GetSubStringPos(const wchar_t* Src, const wchar_t* SubStr, size_t Sub) {
    return this->GetSubStringPos(std::wstring(Src), SubStr, Sub);
}

int Split::GetSubStringPos(const std::wstring& Src, const wchar_t* SubStr, size_t Sub, bool SaveAll) {
    this->MatchedStr.clear();

    const auto bSaveFirst = Sub == static_cast<size_t>(-1);

    const wregex SubString(SubStr, this->Flag);
    wsmatch matches;

    std::wstring::const_iterator StrBegin = Src.begin();
    const std::wstring::const_iterator StrEnd = Src.end();

    size_t CurPos = 0;
    size_t PrePos = 0;
    for (size_t i = 0;
        regex_search(StrBegin, StrEnd, matches, SubString) && (i <= Sub);
        i++,
        PrePos = Src.length() - matches.suffix().length(),
        StrBegin = matches[0].second) {
        CurPos = PrePos + matches.position(0u);

        if (SaveAll) {
            this->SubStringVec.emplace_back(matches[0]);
        }
        if (bSaveFirst) {
            this->MatchedStr = matches[0];
        }
        if (i == Sub) {
            this->MatchedStr = matches[0];

            return static_cast<int>(CurPos);
        }
    }

    return bSaveFirst
        ? static_cast<int>(CurPos)
        : -1;
}

void Split::GetAllSubString(const wchar_t* SubStr) {
    this->GetAllSubString(this->GetSplitData(), SubStr);
}

void Split::GetAllSubString(const wchar_t* Src, const wchar_t* SubStr) {
    this->GetAllSubString(std::wstring(Src), SubStr);
}

void Split::GetAllSubString(const std::wstring& Src, const wchar_t* SubStr) {
    this->SubStringVec.clear();
    this->GetSubStringPos(Src, SubStr, -1, true);
}

const wchar_t* Split::GetNextKeyWord(size_t StartPos) const {
    //match all in this case
    return this->GetNextKeyWord(StartPos, ALL);
}

const wchar_t* Split::GetNextKeyWord(size_t StartPos, const wchar_t* pKeyWord) const {
    //valid current pos    
    if (!(!this->KeyWordPairVec.empty() && (StartPos < this->KeyWordPairVec.back().first))) {
        return nullptr;
    }

    const wregex KW(pKeyWord, this->Flag);

    for (auto& it : this->KeyWordPairVec) {
        if ((it.first > StartPos) && regex_match(it.second.c_str(), KW))
            return it.second.c_str();
    }

    return nullptr;
}

int Split::GetNextKeyWordPos(size_t StartPos) const {
    //match all in this case
    return this->GetNextKeyWordPos(StartPos, ALL);
}

int Split::GetNextKeyWordPos(size_t startPos, const wchar_t* pKeyWord) const {
    //valid current pos
    if (!(!this->KeyWordPairVec.empty() && (startPos < this->KeyWordPairVec.back().first))) {
        return -1;
    }

    const wregex keywordRegex(pKeyWord, this->Flag);

    for (const auto& [pos, keyword] : this->KeyWordPairVec) {
        if ((pos > startPos) && regex_match(keyword.c_str(), keywordRegex))
            return static_cast<int>(pos);
    }

    return -1;
}

bool Split::InsertFile(const Split& newFile,
    size_t pos, const bool bReplace) {
    if (pos >= this->SplitStrVec.size()) {
        pos = this->SplitStrVec.size() - 1;
    }

    // ------------
    // insert vec
    // ------------

    const auto newVec = newFile.GetSplitVec();
    const auto where = this->SplitStrVec.begin() + static_cast<int>(pos);

    this->SplitStrVec.insert(where + 1, newVec->begin(), newVec->end());

    if (bReplace && where != this->SplitStrVec.end()) {
        this->SplitStrVec.erase(where);
    }

    // ------------
    // insert keyword
    // ------------

    // copy
    auto newKeywordVec = *newFile.GetKeyWordVec();

    // offset old
    auto prevIt = KeyWordPairVec.begin();
    bool bFirst = false;

    for (auto it = KeyWordPairVec.begin(); it != KeyWordPairVec.end(); ++it) {
        if (it->first > pos) {
            it->first += newVec->size() - 1;

            if (!bFirst && prevIt == KeyWordPairVec.begin()) {
                if (it == KeyWordPairVec.begin()) {
                    bFirst = true;

                    continue;
                }

                prevIt = it - 1;
            }
        }
    }

    // insert new
    for (auto& newKeywordPos : newKeywordVec | std::views::keys) {
        newKeywordPos += pos;
    }

    KeyWordPairVec.insert(prevIt + !bFirst, newKeywordVec.begin(), newKeywordVec.end());

    return true;
}
