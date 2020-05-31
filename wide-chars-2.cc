#include <locale>
#include <iostream>
#include <string>
#ifdef WINDOWSLIB
#include <Windows.h>
#endif
using namespace std::literals::string_literals;
std::string WidestringToString(const std::wstring& wstr, const std::string& locale)
{
if (wstr.empty())
{
return std::string();
}
size_t pos;
size_t begin = 0;
std::string ret;
size_t size;
#ifdef WINDOWSLIB
_locale_t lc = _create_locale(LC_ALL, locale.c_str());
pos = wstr.find(static_cast<wchar_t>(0), begin);
while (pos != std::wstring::npos && begin < wstr.length())
{
std::wstring segment = std::wstring(&wstr[begin], pos - begin);
_wcstombs_s_l(&size, nullptr, 0, &segment[0], _TRUNCATE, lc);
std::string converted = std::string(size, 0);
_wcstombs_s_l(&size, &converted[0], size, &segment[0], _TRUNCATE, lc);
ret.append(converted);
begin = pos + 1;
pos = wstr.find(static_cast<wchar_t>(0), begin);
}
if (begin <= wstr.length()) {
std::wstring segment = std::wstring(&wstr[begin], wstr.length() - begin);
_wcstombs_s_l(&size, nullptr, 0, &segment[0], _TRUNCATE, lc);
std::string converted = std::string(size, 0);
_wcstombs_s_l(&size, &converted[0], size, &segment[0], _TRUNCATE, lc);
converted.resize(size - 1);
ret.append(converted);
}
_free_locale(lc);
#elif defined LINUXLIB
std::string currentLocale = setlocale(LC_ALL, nullptr);
setlocale(LC_ALL, locale.c_str());
pos = wstr.find(static_cast<wchar_t>(0), begin);
while (pos != std::wstring::npos && begin < wstr.length())
{
std::wstring segment = std::wstring(&wstr[begin], pos - begin);
size = wcstombs(nullptr, segment.c_str(), 0);
std::string converted = std::string(size, 0);
wcstombs(&converted[0], segment.c_str(), converted.size());
ret.append(converted);
ret.append({ 0 });
begin = pos + 1;
pos = wstr.find(static_cast<wchar_t>(0), begin);
}
if (begin <= wstr.length()) {
std::wstring segment = std::wstring(&wstr[begin], wstr.length() - begin);
size = wcstombs(nullptr, segment.c_str(), 0);
std::string converted = std::string(size, 0);
wcstombs(&converted[0], segment.c_str(), converted.size());
ret.append(converted);
}
setlocale(LC_ALL, currentLocale.c_str());
#elif defined MACOSLIB
#endif
return ret;
}