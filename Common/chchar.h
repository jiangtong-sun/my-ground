#ifndef CHCHAR_H
#define CHCHAR_H

#include "commacro.h"
#include <string>

enum Character {
    ChineseLeftSquareBracket,
    ChineseLeftParenthesis,
    ChineseRightParenthesis,
    ChineseRightSquareBracket
};

#define DEFAULT_FONT_SIZE 16
#define DEFAULT_FONT_NAME "Arial"
//#define DEFAULT_FONT_NAME "Microsoft YaHei UI"
COMMON_EXPORT const char *getCharEncoding(Character chinesechar);
COMMON_EXPORT const wchar_t * getWchar(Character chinesechar);
COMMON_EXPORT std::string Wstring2String(const std::wstring& wstr);
COMMON_EXPORT std::wstring String2Wstring(const std::string& wstr);

#endif // CHCHAR_H
