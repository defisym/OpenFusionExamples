#include "StringConverter.h"

STRINGCONVERTER_API char *StringConverter_WideCharToCodePage(const widechar *str, unsigned int CodePage)
{
	int size = lstrlen(str) * 4;
	char *ret = new char[size];

	ret[WideCharToMultiByte(CodePage, 0, str, lstrlen(str), ret, size, NULL, NULL)] = 0;

	return ret;
}

STRINGCONVERTER_API wchar_t *StringConverter_MultiByteToCodePage(const multibyte *str, unsigned int CodePage)
{
	int size = strlen(str) * 2;
	wchar_t *ret = new wchar_t[size];
	ret[MultiByteToWideChar(CodePage, 0, str, strlen(str), ret, size)] = 0;

	return ret;
}

STRINGCONVERTER_API utf8 *StringConverter_MultiByteToUTF8(const multibyte *str)
{
	wchar_t *temp = StringConverter_MultiByteToWideChar(str);
	int size = lstrlen(temp) * 4;
	utf8 *ret = new char[size];

	ret[WideCharToMultiByte(CP_UTF8, 0, temp, lstrlen(temp), ret, size, NULL, NULL)] = 0;
	StringConverter_Free(temp);

	return ret;
}

STRINGCONVERTER_API multibyte *StringConverter_UTF8ToMultiByte(const utf8 *str)
{
	wchar_t *temp = StringConverter_UTF8ToWideChar(str);
	int size = lstrlen(temp) * 4;
	multibyte *ret = new char[size];

	ret[WideCharToMultiByte(CP_ACP, 0, temp, lstrlen(temp), ret, size, NULL, NULL)] = 0;
	StringConverter_Free(temp);

	return ret;
}

STRINGCONVERTER_API void StringConverter_Free(void *str)
{
	delete[] str;
	str = 0;
}