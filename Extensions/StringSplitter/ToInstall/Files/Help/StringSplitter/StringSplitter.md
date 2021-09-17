# String Splitter

## Description

This object is a string splitter base on C++ regex, please check the links below to get more information about how to use regular expression:

EN:<https://en.wikipedia.org/wiki/Regular_expression>

ZH:<https://github.com/EZLippi/practical-programming-books/blob/master/src/30-minutes-to-learn-regex.md>

## Properties

This object has no properties.

## Action

- Reset Splitter

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
  - Iterate KeyWordMap

- SubString
  - Get All SubString (Matchable)
  - Iterate Matched SubString

## Condition

- Iterate
  - On Iterate SplitStrVec
  - On Iterate KeyWordMap

  - On Iterate Matched SubString

## Expression

- Replace String (Matchable)

- SubString (Matchable)
  - Get Nst Pos in Str

  - Get Last Match Result
  - Get Match Result in Str

  - Get SubStringVec Size
  - Get SubString in SubStringVec

- SplitStr
  - Get SplitStrVec Size
  - Get SPlitStr in SplitStrVec

- Keyword
  - Get Next Keyword Pos (Matchable)

  - Get KeyWordMap Size
  - Get Keyword in KeyWordMap
