/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include "str.h"
#include <tchar.h>
#include "internals/tstring.h"

namespace wl {

// Utilities to file path operations with wl::tstring.
namespace path {

inline wl::tstring& trim_backslash(wl::tstring& filePath) {
	while (filePath.back() == _T('\\')) {
		filePath.resize(filePath.length() - 1);
	}
	return filePath;
}

inline bool is_same(const wl::tstring& filePath, const TCHAR* other) noexcept {
	return !lstrcmpi(filePath.c_str(), other);
}

inline bool is_same(const wl::tstring& filePath, const wl::tstring& other) noexcept {
	return !lstrcmpi(filePath.c_str(), other.c_str());
}

inline bool has_extension(const wl::tstring& filePath, const TCHAR* extension) noexcept {
	if (extension[0] == _T('.')) { // extension starts with dot, compare right away
		return str::ends_withi(filePath, extension);
	}

	TCHAR dotExtension[32] = _T("."); // arbitrary buffer length
	lstrcat(dotExtension, extension);
	return str::ends_withi(filePath, dotExtension);
}

inline bool has_extension(const wl::tstring& filePath, const wl::tstring& extension) noexcept {
	return has_extension(filePath, extension.c_str());
}

inline bool has_extension(const wl::tstring& filePath, std::initializer_list<const TCHAR*> extensions) noexcept {
	for (const TCHAR* ext : extensions) {
		if (has_extension(filePath, ext)) {
			return true;
		}
	}
	return false;
}

inline wl::tstring& change_extension(wl::tstring& filePath, const TCHAR* newExtension) {
	size_t dotIdx = filePath.find_last_of(_T('.'));
	if (dotIdx != wl::tstring::npos) { // filePath already have an extension
		filePath.resize(dotIdx + 1); // truncate after the dot
	} else { // filePath doesn't have an extension
		filePath.append(1, _T('.'));
	}
	filePath.append(newExtension[0] == _T('.') ? newExtension + 1 : newExtension);
	return filePath;
}

inline wl::tstring& change_extension(wl::tstring& filePath, const wl::tstring& newExtension) {
	return change_extension(filePath, newExtension.c_str());
}

inline wl::tstring folder_from(const wl::tstring& filePath) {
	wl::tstring ret = filePath;
	size_t found = ret.find_last_of(_T('\\')); // won't include trailing backslash
	if (found != wl::tstring::npos) {
		ret.resize(found);
	}
	return ret;
}

inline wl::tstring file_from(const wl::tstring& filePath) {
	wl::tstring ret = filePath;
	size_t found = ret.find_last_of(_T('\\'));
	if (found != wl::tstring::npos) {
		ret.erase(0, found + 1);
	}
	return ret;
}

}//namespace path
}//namespace wl
