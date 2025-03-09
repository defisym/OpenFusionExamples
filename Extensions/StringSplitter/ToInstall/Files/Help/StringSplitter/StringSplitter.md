# String Splitter

## Description

This object is a string splitter base on C++ regex, please check the links below to get more information about how to use regular expression:

EN:<https://en.wikipedia.org/wiki/Regular_expression>

ZH:<https://github.com/EZLippi/practical-programming-books/blob/master/src/30-minutes-to-learn-regex.md>

## Properties

This object has no properties.

## Note

- if you input `\` in fusion, ext will get `\\`, because Windows use `\\` to escape `\`. that is to say, if you want to use `\s`, just input `\s` in fusion instead of `\\s` like you do when setting with `std::wregex regex = L"\\s"` in cpp.
- if you need to match `\`, you need to input `\\` in regex to match it, cause ext get `\\` as string, and you need `\\\\` to match it.
- if you need to match `NewLine`(AKA`\r\n`), just simply input `\r\n` in fusion, ext will escape `\\r\\n` it got to `\r\n`, or you can use `NewLine$` with multiple `+` to append strings.

## Action

- Reset Split
  - *reset object if needed*

- Load From File
  - *if the file is not [encrypted](https://github.com/defisym/OpenFusionExamples/tree/master/Extensions/Encrypter/ToInstall/Files), keep the key section empty.*  
  - *you need to define the file code page, 1 = `Unicode`, 0 = `ANSI`*
  - *this action will automatically start split if auto split is set to on*
- Load From String
  - *load from string, e.g.`Hello/r/nWorld`*
  - *this action will automatically start split if auto split is set to on*

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
      - *1 = Case Insensitive, 0 = Case Sensitive*
      - *perform an or/xor icase to flag*

  - Set Regex
    - Set All Regex
      - *set all five regex at the same time, has the same effect if you set them separately use the actions below*
      - *if you set regex will active it's flag to take effect in `Split Data` action*
      - *if you don't want any of this regex work, just keep section blank*

    - Set Split Regex
      - *main regex, use this to split data, e.g. `\r\n`, ext will split by lines.*
    - Set EmptyLine Regex
      - *if you want to remove empty lines or some lines, set this regex, e.g. `^\s*$|#begin.*`, remove empty lines and lines contain `#begin`*
    - Set Comment Regex
      - *if you want to remove comments in data, set this regex. e.g. C++ style comments `(\/\*(.|[\r\n])*?\*\/)|(\/\/[^\r\n]*)`*
    - Set Indent Regex
      - *remove indents of a line, aka all blank characters in the beginning or ending, e.g. `^(\s*)|(\s*)$`*
    - Set Keyword Regex
      - *add a line(which match this regex)'s content and it's line number to a vector, and you can get them later by keyword actions/conditions/expressions*

- Split
  - Auto Split
    - *1 = Enable, 0 = Disable, default is disable*
    - *ext will automatically split data when you load them, or you need to use `Split Data` to start split*

  - Split Data
    - *split data with regex defined above.*
    - *cost about 35ms for a 1000 line file*
    - *routine*
      - *remove comments*
      - *iterate all lines split by split regex*
      - *if a line is empty, skip it*
      - *remove indent*
      - *save result to SplitStrVec*
      - *if current line matches keyword, save it and it's line number after split*

  - Iterate SplitStrVec
    - *iterate split results*
  - Iterate KeyWordPairVec
    - *iterate keywords*

- SubString
  - Get All Matched SubString
    - *Keep src empty to match source string*
  - Iterate Matched SubString
    - *iterate substrings matched by `Get All Matched SubString`*

- Replace Each
  - *this set of A/C/Es are used to replace each relative to matched substring. e.g. if you want to replace `<num>` to the num-st alterable value, you need to use this function. sadly `SubString` functions couldn't do this.*

  - Iterate Replace Each
    - *start a for each loop to iterate all substrings that match regex*
    - *use `On Iterate Replace Each` condition to start process loop*
    - *use `Get Current Match` to get current match result, then use `Set Replace String` to set replace*
  - Iterate Replace Each All
    - *start a for each loop to iterate all substrings that match regex, but will check the replaced string and loop until no match in replaced string or nothing replaced this loop*
    - *used for nest replace, e.g., `<Keep><LocalCodeDisplayName_<LocalCode>>`, match `(<)([^<>]+?)(>)`, replace rule is replace `<LocalCode>` to `EN`, `<LocalCodeDisplayName_EN>` to `English`, the replace result is `<Keep>English`*
  - Set Replace String

## Condition

- String Match Regex
  - *return true if string matches regex*
  - *Keep src empty to search source string*
- String Has Regex
  - *return true if string's substring matches regex*
  - *Keep src empty to search source string*

- Iterate
  - On Iterate SplitStrVec
    - *you can get current split str here*
  - On Iterate KeyWordPairVec
    - *you can get current KeyWord here*

  - On Iterate Matched SubString
    - *you can get current SubString here*

  - On Iterate Replace Each
    - *iterate each matched result*
    - *please read `Iterate Replace Each` section to get more information about `Replace Each`*

## Expression

- Get Split Data
  - *Get the string that removed comment*
  - *if data not split before this (or auto split not enabled), ext will return source data instead*

- Replace String (Matchable)
  - *replace string with given regex*
  - *Keep src empty to replace source string*

- Replace Each
  - *please read `Iterate Replace Each` section to get more information about `Replace Each`*
  - Get Current Match
    - *Get current match subtring, you can do some work with it to get the replace string you want*
  - Get Replace Result

- SplitStr
  - Get SplitStrVec Size
  - Get Nth SplitStr in SplitStrVec
    - *0-based*

  - Iterate: Get Current SplitStr

- Keyword
  - Get Next Keyword (Matchable)
  - Get Next Keyword Pos (Matchable)
    - *get the line number of next matched keyword after given position. if you keep the regex section empty, ext will return next keyword after given position*

  - Get KeyWordPairVec Size
  - Get Nth Keyword in KeyWordPairVec
    - *0-based*
  - Get Nth Keyword Pos in KeyWordPairVec
    - *0-based*

  - Iterate: Get Current KeyWord
  - Iterate: Get Current KeyWord Pos

- SubString (Matchable)
  - Get Nth SubString Position in Str
    - *Keep src empty to search source string*
    - *0-based*

  - Get Last Matched SubString
  - Get Nth Matched SubString in Str
    - *Keep src empty to search source string*
    - *0-based*

  - Get SubStringVec Size
  - Get Nth SubString in SubStringVec
    - *0-based*

  - Iterate: Get Current SubString
