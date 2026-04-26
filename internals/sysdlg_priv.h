/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <vector>
#include <Windows.h>
#include <tchar.h>

namespace wl {
namespace _wli {
namespace sysdlg_priv {

template<typename T>
struct static_holder final {
	static T val; // http://stackoverflow.com/a/11709860
};

template<typename T> T static_holder<T>::val;

static static_holder<HHOOK> hHookMsgBox; // global hooks
static static_holder<HWND>  hWndParent;

inline std::vector<TCHAR> format_file_filter(const TCHAR* filterWithPipes) {
	// Input filter follows same C# syntax:
	// L"Text Files (*.txt)|*.txt|All Files (*.*)|*.*"
	std::vector<TCHAR> ret(lstrlen(filterWithPipes) + 2, _T('\0')); // two terminating nulls
	for (size_t i = 0; i < ret.size() - 1; ++i) {
		ret[i] = (filterWithPipes[i] != _T('|')) ? filterWithPipes[i] : _T('\0');
	}
	return ret;
}

}//namespace sysdlg_priv
}//namespace _wli
}//namespace wl
