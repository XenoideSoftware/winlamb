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

// CBT hook used by sysdlg::msgbox to recenter the message box on its parent.
// Declared as a free WINAPI function (instead of an inline lambda) so its
// calling convention matches HOOKPROC on 32-bit MinGW/TDM-GCC builds, where
// captureless lambdas decay to a __cdecl pointer that cannot implicitly
// convert to the __stdcall HOOKPROC signature.
inline LRESULT CALLBACK msgbox_cbt_proc(int code, WPARAM wp, LPARAM lp) {
    // http://www.codeguru.com/cpp/w-p/win32/messagebox/print.php/c4541
    if (code == HCBT_ACTIVATE) {
        HWND hMsgbox = reinterpret_cast<HWND>(wp);
        RECT rcMsgbox{}, rcParent{};

        if (hMsgbox &&
            hWndParent.val &&
            GetWindowRect(hMsgbox, &rcMsgbox) &&
            GetWindowRect(hWndParent.val, &rcParent))
        {
            RECT  rcScreen{};
            POINT pos{};
            SystemParametersInfo(SPI_GETWORKAREA, 0,
                static_cast<PVOID>(&rcScreen), 0); // size of desktop

            // Adjusted x,y coordinates to message box window.
            pos.x = rcParent.left + (rcParent.right - rcParent.left) / 2
                - (rcMsgbox.right - rcMsgbox.left) / 2;
            pos.y = rcParent.top + (rcParent.bottom - rcParent.top) / 2
                - (rcMsgbox.bottom - rcMsgbox.top) / 2;

            // Screen out-of-bounds corrections.
            if (pos.x < 0) {
                pos.x = 0;
            } else if (pos.x + (rcMsgbox.right - rcMsgbox.left) > rcScreen.right) {
                pos.x = rcScreen.right - (rcMsgbox.right - rcMsgbox.left);
            }
            if (pos.y < 0) {
                pos.y = 0;
            } else if (pos.y + (rcMsgbox.bottom - rcMsgbox.top) > rcScreen.bottom) {
                pos.y = rcScreen.bottom - (rcMsgbox.bottom - rcMsgbox.top);
            }
            MoveWindow(hMsgbox, pos.x, pos.y,
                rcMsgbox.right - rcMsgbox.left,
                rcMsgbox.bottom - rcMsgbox.top,
                FALSE);
        }
        UnhookWindowsHookEx(hHookMsgBox.val); // release global hook
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

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
