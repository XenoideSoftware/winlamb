/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <cwctype>
#include <string>
#include <Windows.h>
#include <tchar.h>
#include "tstring.h"

namespace wl {
namespace _wli {
namespace str_priv {

template<typename T>
inline T format_raw_arg(T val) noexcept {
#ifdef _UNICODE
	static_assert(!std::is_same<T, const char*>::value,
		"Non-wide char* being used on str::format(), str::to_tstring() can fix it.");
	static_assert(!std::is_same<T, std::string>::value,
		"Non-wide std::string being used on str::format(), str::to_tstring() can fix it.");
#else
	static_assert(!std::is_same<T, const wchar_t*>::value,
		"Wide wchar_t* being used on str::format(), str::to_tstring() can fix it.");
	static_assert(!std::is_same<T, std::wstring>::value,
		"Wide std::wstring being used on str::format(), str::to_tstring() can fix it.");
#endif
	return val;
}

inline const TCHAR* format_raw_arg(const wl::tstring& val) noexcept {
	return val.c_str(); // so format() will also accept tstring in formatting list
}

template<typename ...argsT>
inline wl::tstring format_raw(size_t /*strFormatLen*/, const TCHAR* strFormat, const argsT&... args) {
	// _sctprintf is missing from TDM-GCC's CRT. Render straight into a
	// generously-sized stack buffer, then trim. 8 KiB is far more than any
	// winlamb caller (button captions, error messages, formatted paths)
	// produces in practice; if the format truncates, we still get a valid
	// (if shorter) string back rather than a broken two-pass dance.
	TCHAR buf[8 * 1024];
	int len = _sntprintf(buf, sizeof(buf) / sizeof(buf[0]),
		strFormat, format_raw_arg(args)...);
	if (len < 0) {
		// Truncation: _sntprintf returns -1 on MSVCRT when the buffer is full.
		len = static_cast<int>(sizeof(buf) / sizeof(buf[0])) - 1;
		buf[len] = _T('\0');
	}
	return wl::tstring(buf, static_cast<size_t>(len));
}

inline bool ends_begins_first_check(const wl::tstring& s, const TCHAR* what, size_t& whatLen) noexcept {
	if (s.empty()) return false;

	whatLen = lstrlen(what);
	if (!whatLen || whatLen > s.length()) {
		return false;
	}
	return true;
}

// Converts a wide string to the project's TCHAR string (no-op under _UNICODE).
inline wl::tstring wide_to_tstring(const wchar_t* data, int len) {
#ifdef _UNICODE
	return wl::tstring(data, static_cast<size_t>(len));
#else
	if (!data || !len) return {};
	int neededLen = WideCharToMultiByte(CP_ACP, 0, data, len, nullptr, 0, nullptr, nullptr);
	wl::tstring ret(static_cast<size_t>(neededLen), '\0');
	WideCharToMultiByte(CP_ACP, 0, data, len, &ret[0], neededLen, nullptr, nullptr);
	return ret;
#endif
}

inline wl::tstring parse_ascii(const BYTE* data, size_t sz) {
	wl::tstring ret;
	if (data && sz) {
		ret.resize(sz);
		for (size_t i = 0; i < sz; ++i) {
			if (!data[i]) { // found terminating null
				ret.resize(i);
				return ret;
			}
			ret[i] = static_cast<TCHAR>(data[i]); // raw conversion (also valid under ANSI)
		}
	}
	return ret; // data didn't have a terminating null
}

inline wl::tstring parse_encoded(const BYTE* data, size_t sz, UINT codePage) {
	wl::tstring ret;
	if (data && sz) {
		int neededLen = MultiByteToWideChar(codePage, 0, reinterpret_cast<const char*>(data),
			static_cast<int>(sz), nullptr, 0);
		std::wstring wide(static_cast<size_t>(neededLen), L'\0');
		MultiByteToWideChar(codePage, 0, reinterpret_cast<const char*>(data),
			static_cast<int>(sz), &wide[0], neededLen);
		wide.resize( lstrlenW(wide.c_str()) ); // trim_nulls
		ret = wide_to_tstring(wide.c_str(), static_cast<int>(wide.length()));
	}
	return ret;
}

}//namespace str_priv
}//namespace wli
}//namespace wl
