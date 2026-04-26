/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <string>
#include <Windows.h>
#include <tchar.h>
#include "tstring.h"

namespace wl {
namespace _wli {

// Public methods that allow get/set window text with wl::tstring.
template<typename derivedT>
class base_text_pubm {
private:
	const HWND& _hWnd;

public:
	base_text_pubm(const HWND& hWnd) noexcept :
		_hWnd(hWnd) { }

	// Simple wrapper to SetWindowText.
	derivedT& set_text(const TCHAR* s) noexcept {
		SetWindowText(this->_hWnd, s);
		return *static_cast<derivedT*>(this);
	}

	// Simple wrapper to SetWindowText.
	derivedT& set_text(const wl::tstring& s) noexcept {
		return this->set_text(s.c_str());
	}

	// Simple wrapper to GetWindowText.
	wl::tstring get_text() const {
		wl::tstring buf;
		int len = GetWindowTextLength(this->_hWnd);
		if (len) {
			buf.resize(len + 1, _T('\0'));
			GetWindowText(this->_hWnd, &buf[0], len + 1);
			buf.resize(len);
		}
		return buf;
	}
};

}//namespace _wli
}//namespace wl
