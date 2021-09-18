# String Splitter

## Description

This object is a string splitter base on C++ regex, please check the links below to get more information about how to use regular expression:

EN:<https://en.wikipedia.org/wiki/Regular_expression>

ZH:<https://github.com/EZLippi/practical-programming-books/blob/master/src/30-minutes-to-learn-regex.md>

## Properties

This object has no properties.

## Action

- Reset Split

- Load From File
- Load From String

- Regex
  - Regex Flag
    - Reset Regex Flag
      - *set flag back to default flag:*

        ```C++
            DefaultFlag = ECMAScript | optimize = 1025
        ```

    - Set Regex Flag
      - *use | to set multiple flags, e.g. set ECMAScript and icase, Flag = 0x01|0x0100 = 257， then input 257. Please check all regex_constants below:*

        ```C++
        enum syntax_option_type : int { // specify RE syntax rules
            ECMAScript = 0x01,
            basic      = 0x02,
            extended   = 0x04,
            awk        = 0x08,
            grep       = 0x10,
            egrep      = 0x20,
            _Gmask     = 0x3F,
        
            icase    = 0x0100,
            nosubs   = 0x0200,
            optimize = 0x0400,
            collate  = 0x0800
        };
        ```

    - Set Case Insensitive
      - *perform an or/xor icase to flag*

  - Set Regex
    - Set All Regex

    - Set Split Regex
    - Set EmptyLine Regex
    - Set Comment Regex
    - Set Indent Regex
    - Set Keyword Regex

- Split
  - Split

  - Iterate SplitStrVec
  - Iterate KeyWordPairVec

- SubString
  - Get All Matched SubString
    - *Keep src empty to match data*
  - Iterate Matched SubString

## Condition

- Iterate
  - On Iterate SplitStrVec
  - On Iterate KeyWordPairVec

  - On Iterate Matched SubString

## Expression

- Replace String (Matchable)

- SubString (Matchable)
  - Get Nst SubString Position in Str

  - Get Last Matched SubString
  - Get Nst Matched SubString in Str

  - Get SubStringVec Size
  - Get Nst SubString in SubStringVec

  - Iterate Get Current SubString

- SplitStr
  - Get SplitStrVec Size
  - Get Nst SplitStr in SplitStrVec

  - Iterate Get Current SplitStr

- Keyword
  - Get Next Keyword Pos (Matchable)

  - Get KeyWordPairVec Size
  - Get Nst Keyword in KeyWordPairVec

  - Iterate Get Current KeyWord
