#include "pch.h"
#include "IniFile.h"
#include <sstream>

IniFile::IniFile(PCWSTR path) : _path(path) {
}

CString IniFile::ReadString(PCWSTR section, PCWSTR name, PCWSTR defaultValue) {
    CString result;
    auto count = ::GetPrivateProfileString(section, name, defaultValue, result.GetBufferSetLength(128), 128, _path);
    return result;
}

int IniFile::ReadInt(PCWSTR section, PCWSTR name, int defaultValue) {
    return ::GetPrivateProfileInt(section, name, defaultValue, _path);
}

COLORREF IniFile::ReadColor(PCWSTR section, PCWSTR name, COLORREF defaultValue) {
    auto text = ReadString(section, name);
    if (text.IsEmpty())
        return defaultValue;

    if (text.Left(2) == L"0x")
        return ParseHexColor(text.Mid(2));

    if (text.Find(L','))
        return ParseDecColor(text);

    return ParseHexColor(text);
}

COLORREF IniFile::ParseHexColor(const CString& hex) {
    std::wstringstream ss;
    DWORD color;
    ss << std::hex << hex;
    ss >> color;
    return color;
}

COLORREF IniFile::ParseDecColor(const CString& text) {
    int start = 0;
    COLORREF color = 0;
    auto str = text.Tokenize(L",", start);
    if (str.IsEmpty())
        return CLR_INVALID;
    color |= _wtoi(str);
    str = text.Tokenize(L",", start);
    if (str.IsEmpty())
        return CLR_INVALID;
    color |= _wtoi(str) << 8;
    str = text.Tokenize(L",", start);
    if (str.IsEmpty())
        return CLR_INVALID;
    color |= _wtoi(str) << 16;

    return color;
}
