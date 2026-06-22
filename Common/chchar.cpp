#include "chchar.h"
#include <string>
#include <windows.h>

std::string Wstring2String(const std::wstring& wstr){
    std::string res;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr);
    if (len <= 0){
        return res;
    }

    char* buffer = new char[len + 1];
    if (buffer == nullptr){
        return res;
    }

    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, nullptr, nullptr);
    buffer[len] = '\0';
    res.append(buffer);
    delete[] buffer;
    return res;
}

std::wstring String2Wstring(const std::string& wstr){
    std::wstring res;
    int len = MultiByteToWideChar(CP_ACP, 0, wstr.c_str(), wstr.size(), nullptr, 0);
    if( len < 0 ){
        return res;
    }

    wchar_t* buffer = new wchar_t[len + 1];
    if( buffer == nullptr){
        return res;
    }

    MultiByteToWideChar(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len);
    buffer[len] = '\0';
    res.append(buffer);
    delete[] buffer;
    return res;
}

const wchar_t * getWchar(Character chinesechar){
    std::wstring specialWords[] = {
        L"【",
        L"（",
        L"）",
        L"】"
    };

    return specialWords[chinesechar].c_str();
}

const char* getCharEncoding(Character c){
    switch(c){
    case ChineseLeftSquareBracket:
        return "\\xE3\\x80\\x90";
    case ChineseLeftParenthesis:
        return "\\xEF\\xBC\\x88";
    case ChineseRightParenthesis:
        return "\\xE3\\xBC\\x89";
    case ChineseRightSquareBracket:
        return "\\xE3\\x80\\x91";
    default:
        return nullptr;
    }
}
