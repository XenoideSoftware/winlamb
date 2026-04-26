/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include "internals/base_native_ctrl_pubm.h"
#include "internals/styler.h"
#include "str.h"
#include "wnd.h"
#include <tchar.h>
#include "internals/tstring.h"

namespace wl {

// Wrapper to native combobox control.
class combobox final :
	public wnd,
	public _wli::base_native_ctrl_pubm<combobox>
{
public:
	enum class sort { SORTED, UNSORTED };

private:
	HWND                   _hWnd = nullptr;
	_wli::base_native_ctrl _baseNativeCtrl{_hWnd};

public:
	// Wraps window style changes done by Get/SetWindowLongPtr.
	_wli::styler<combobox> style{this};

	combobox() noexcept :
		wnd(_hWnd), base_native_ctrl_pubm(_baseNativeCtrl) { }

	combobox(combobox&&) = default;
	combobox& operator=(combobox&&) = default; // movable only

	combobox& create(HWND hParent, int ctrlId, POINT pos,
		LONG width, sort sortType)
	{
		this->_baseNativeCtrl.create(hParent, ctrlId, nullptr,
			pos, {width, 0}, _T("combobox"),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST |
			(sortType == sort::SORTED ? CBS_SORT : 0), 0);
		return *this;
	}

	combobox& create(const wnd* parent, int ctrlId, POINT pos,
		LONG width, sort sortType)
	{
		return this->create(parent->hwnd(), ctrlId, pos, width, sortType);
	}

	size_t count() const noexcept {
		return SendMessage(this->_hWnd, CB_GETCOUNT, 0, 0);
	}

	size_t get_selected_index() const noexcept {
		return static_cast<size_t>(SendMessage(this->_hWnd, CB_GETCURSEL, 0, 0));
	}

	combobox& remove_all() noexcept {
		SendMessage(this->_hWnd, CB_RESETCONTENT, 0, 0);
		return *this;
	}

	combobox& add(const TCHAR* entries, TCHAR delimiter = _T('|')) {
		TCHAR delim[2]{delimiter, _T('\0')};
		std::vector<wl::tstring> vals = str::split(entries, delim);
		for (const wl::tstring& s : vals) {
			SendMessage(this->_hWnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(s.c_str()));
		}
		return *this;
	}

	combobox& add(std::initializer_list<const TCHAR*> entries) noexcept {
		for (const TCHAR* s : entries) {
			SendMessage(this->_hWnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(s));
		}
		return *this;
	}

	wl::tstring get_text(size_t index) const {
		wl::tstring buf;
		size_t len = SendMessage(this->_hWnd, CB_GETLBTEXTLEN, index, 0);
		if (len) {
			buf.resize(len, _T('\0'));
			SendMessage(this->_hWnd, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(&buf[0]));
			buf.resize(len);
		}
		return buf;
	}

	wl::tstring get_selected_text() const {
		return this->get_text(this->get_selected_index());
	}

	combobox& select(size_t index) noexcept {
		SendMessage(this->_hWnd, CB_SETCURSEL, index, 0);
		return *this;
	}
};

}//namespace wl
