/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include "base_msg.h"
#include "base_scroll.h"
#include "../font.h"

namespace wl {
namespace _wli {

// Common ground to all dialog windows.
class base_dialog final {
public:
	// Variables to be set by user, used only during window creation.
	struct setup_vars {
		int dialogId = 0;
	};

private:
	HWND&              _hWnd;
	base_msg<INT_PTR>& _baseMsg;

public:
	~base_dialog() {
		if (this->_hWnd) {
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0500
			SetWindowLongPtrW(this->_hWnd, GWLP_USERDATA, 0);
#else
			SetWindowLongW(this->_hWnd, GWL_USERDATA, 0);
#endif
		}
	}

	base_dialog(HWND& hWnd, base_msg<INT_PTR>& baseMsg) noexcept :
		_hWnd(hWnd), _baseMsg(baseMsg) { }

	// Wrapper to CreateDialogParam.
	HWND create_dialog_param(const setup_vars& setup, HWND hParent) {
		this->_basic_initial_checks(setup);
		return CreateDialogParamW(
			hParent ?
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0500
				reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hParent, GWLP_HINSTANCE)) :
#else
				reinterpret_cast<HINSTANCE>(GetWindowLongW(hParent, GWL_HINSTANCE)) :
#endif
				GetModuleHandle(nullptr),
			MAKEINTRESOURCEW(setup.dialogId), hParent, _dialog_proc,
			reinterpret_cast<LPARAM>(this));
	}

	// Wrapper to DialogBoxParam.
	INT_PTR dialog_box_param(const setup_vars& setup, HWND hParent) {
		this->_basic_initial_checks(setup);
		return DialogBoxParamW(
			hParent ?
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0500
				reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hParent, GWLP_HINSTANCE)) :
#else
				reinterpret_cast<HINSTANCE>(GetWindowLongW(hParent, GWL_HINSTANCE)) :
#endif
				GetModuleHandle(nullptr),
			MAKEINTRESOURCEW(setup.dialogId), hParent, _dialog_proc,
			reinterpret_cast<LPARAM>(this));
	}

private:
	void _basic_initial_checks(const setup_vars& setup) const {
		if (this->_hWnd) {
			throw std::logic_error("Tried to create dialog twice.");
		}
		if (!setup.dialogId) {
			throw std::logic_error("No dialog resource ID given on this->setup.dialogId.");
		}
	}

	static INT_PTR CALLBACK _dialog_proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) noexcept {
		base_dialog* pSelf = nullptr;
		INT_PTR ret = FALSE; // default for non-processed messages

		if (msg == WM_INITDIALOG) {
			pSelf = reinterpret_cast<base_dialog*>(lp);
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0500
			SetWindowLongPtrW(hDlg, DWLP_USER, reinterpret_cast<LONG_PTR>(pSelf));
#else
			SetWindowLongW(hDlg, DWL_USER, reinterpret_cast<LONG>(pSelf));
#endif
			font::util::set_ui_on_children(hDlg); // if user creates controls manually, font must be set manually on them
			pSelf->_hWnd = hDlg; // store HWND
		} else {
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0500
			pSelf = reinterpret_cast<base_dialog*>(GetWindowLongPtrW(hDlg, DWLP_USER));
#else
			pSelf = reinterpret_cast<base_dialog*>(GetWindowLongW(hDlg, DWL_USER));
#endif
		}

		if (pSelf) {
			std::pair<bool, INT_PTR> procRet = pSelf->_baseMsg.process_msg(msg, wp, lp); // catches all message exceptions internally
			if (procRet.first) {
				ret = procRet.second; // message was processed
			}
		}

		if (msg == WM_INITDIALOG) {
			base_scroll::apply_behavior(pSelf->_hWnd);
		} else if (msg == WM_NCDESTROY) { // cleanup
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0500
			SetWindowLongPtrW(hDlg, DWLP_USER, 0);
#else
			SetWindowLongW(hDlg, DWL_USER, 0);
#endif
			if (pSelf) {
				pSelf->_hWnd = nullptr; // clear HWND
			}
		}

		return ret;
	}
};

}//namespace _wli
}//namespace wl
