/* ------------------------------
* Copyright (c) 2013 Lee. All rights reserved.
* Module Name: StringConverter.h
* Version: 1.0
* Abstract: Include file for Windows String Convert Library (MultiByte - UTF8 - WideChar)
* ------------------------------ */

#ifndef StringConverter_h_
#define StringConverter_h_

#include <windows.h>


#define STRINGCONVERTER_API extern "C"

/* --------------------
* Type Definitions
* multibyte : ANSI
* utf8 : UTF-8
* widechar : Unicode
* -------------------- */

typedef char multibyte;
typedef char utf8;
typedef wchar_t widechar;



/* ---------- Core API Functions ---------- */

/* Convert Unicode to specified codepage */
STRINGCONVERTER_API char *StringConverter_WideCharToCodePage(const widechar *str, unsigned int CodePage);

/* Convert ANSI to specified codepage */
STRINGCONVERTER_API wchar_t *StringConverter_MultiByteToCodePage(const multibyte *str, unsigned int CodePage);

/* Convert ANSI to UTF-8 */
STRINGCONVERTER_API utf8 *StringConverter_MultiByteToUTF8(const multibyte *str);

/* Convert UTF-8 to ANSI */
STRINGCONVERTER_API multibyte *StringConverter_UTF8ToMultiByte(const utf8 *str);



/* ---------- Macro Functions ---------- */

/* Convert Unicode to ANSI */
#define StringConverter_WideCharToMultiByte(str) StringConverter_WideCharToCodePage(str, CP_ACP)

/* Convert ANSI to Unicode */
#define StringConverter_MultiByteToWideChar(str) StringConverter_MultiByteToCodePage(str, CP_ACP)

/* Convert Unicode to UTF-8 */
#define StringConverter_WideCharToUTF8(str) StringConverter_WideCharToCodePage(str, CP_UTF8)

/* Convert UTF-8 to Unicode */
#define StringConverter_UTF8ToWideChar(str) StringConverter_MultiByteToCodePage(str, CP_UTF8)



/* ---------- Clean up ---------- */

/* Release the allocated memory */
STRINGCONVERTER_API void StringConverter_Free(void *str);

#endif /* StringConverter_h_ */