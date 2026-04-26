/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <cstdlib>
#include <string>
#include <tchar.h>
#include <Windows.h>
#include "compat.h"

namespace wl {

// String type that follows the project's character mode:
// std::wstring under _UNICODE, std::string otherwise.
using tstring = std::basic_string<TCHAR>;

// TCHAR-aware std::to_wstring / std::to_string.
inline tstring to_tstring(int value) {
#ifdef _UNICODE
	return std::to_wstring(value);
#else
	return std::to_string(value);
#endif
}
inline tstring to_tstring(unsigned int value) {
#ifdef _UNICODE
	return std::to_wstring(value);
#else
	return std::to_string(value);
#endif
}
inline tstring to_tstring(long value) {
#ifdef _UNICODE
	return std::to_wstring(value);
#else
	return std::to_string(value);
#endif
}
inline tstring to_tstring(unsigned long value) {
#ifdef _UNICODE
	return std::to_wstring(value);
#else
	return std::to_string(value);
#endif
}
inline tstring to_tstring(long long value) {
#ifdef _UNICODE
	return std::to_wstring(value);
#else
	return std::to_string(value);
#endif
}
inline tstring to_tstring(unsigned long long value) {
#ifdef _UNICODE
	return std::to_wstring(value);
#else
	return std::to_string(value);
#endif
}

// Converts a TCHAR string to a wide string (no-op under _UNICODE).
// Useful when calling intrinsically wide APIs (BSTR, COM, WinHTTP, etc.).
inline std::wstring to_wstring(const TCHAR* s, int len = -1) {
#ifdef _UNICODE
	return (len < 0) ? std::wstring(s) : std::wstring(s, static_cast<size_t>(len));
#else
	if (!s) return {};
	if (len < 0) len = lstrlenA(s);
	if (!len) return {};
	int needed = MultiByteToWideChar(CP_ACP, 0, s, len, nullptr, 0);
	std::wstring ret(static_cast<size_t>(needed), L'\0');
	MultiByteToWideChar(CP_ACP, 0, s, len, &ret[0], needed);
	return ret;
#endif
}
inline std::wstring to_wstring(const tstring& s) {
	return to_wstring(s.c_str(), static_cast<int>(s.length()));
}

// TCHAR-aware std::stoi / std::stoul.
inline int stoi_t(const tstring& s, size_t* idx = nullptr, int base = 10) {
	TCHAR* end = nullptr;
	long val = _tcstol(s.c_str(), &end, base);
	if (idx) *idx = static_cast<size_t>(end - s.c_str());
	return static_cast<int>(val);
}
inline unsigned long stoul_t(const tstring& s, size_t* idx = nullptr, int base = 10) {
	TCHAR* end = nullptr;
	unsigned long val = _tcstoul(s.c_str(), &end, base);
	if (idx) *idx = static_cast<size_t>(end - s.c_str());
	return val;
}

}//namespace wl
