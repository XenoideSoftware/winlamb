/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include "base_msg.h"

#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0501
// VsStyle.h / Uxtheme.h ship with the Visual Studio SDK and modern
// mingw-w64, but TDM-GCC's frozen MinGW headers omit them. internals/compat.h
// supplies LVP_LISTGROUP and the uxtheme function prototypes when they're
// missing; only pull the real headers when the toolchain actually has them.
#include "compat.h"
#if defined(__has_include)
    #if __has_include(<VsStyle.h>)
        #include <VsStyle.h>
    #endif
    #if __has_include(<Uxtheme.h>)
        #include <Uxtheme.h>
    #endif
#elif defined(_MSC_VER)
    #include <VsStyle.h>
    #include <Uxtheme.h>
#endif
#include <tchar.h>
#if defined(_MSC_VER)
#pragma comment(lib, "UxTheme.lib")
#endif
#endif

namespace wl {
namespace _wli {

// Common ground to all user-custom controls.
template<typename retT, retT RET_VAL>
class base_user_ctrl final {
private:
	base_msg<retT>& _baseMsg;

public:
	base_user_ctrl(base_msg<retT>& baseMsg) :
		_baseMsg(baseMsg)
	{
		baseMsg.msgs.add(WM_NCPAINT, [this](params p) noexcept -> retT {
			this->_paint_themed_borders(p);
			return RET_VAL; // 0 for windows, TRUE for dialogs
		});
	}

private:
	void _paint_themed_borders(const params& p) const noexcept {
		DefWindowProc(this->_baseMsg.hwnd(), WM_NCPAINT, p.wParam, p.lParam); // make system draw the scrollbar for us

#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0501
		if (!(GetWindowLongPtr(this->_baseMsg.hwnd(), GWL_EXSTYLE) & WS_EX_CLIENTEDGE) ||
			!IsThemeActive() ||
			!IsAppThemed() ) return;

		RECT rc{};
		GetWindowRect(this->_baseMsg.hwnd(), &rc); // window outmost coordinates, including margins
		ScreenToClient(this->_baseMsg.hwnd(), reinterpret_cast<POINT*>(&rc));
		ScreenToClient(this->_baseMsg.hwnd(), reinterpret_cast<POINT*>(&rc.right));
		rc.left += 2; rc.top += 2; rc.right += 2; rc.bottom += 2; // because it comes up anchored at -2,-2

		RECT rc2{}; // clipping region; will draw only within this rectangle
		HDC hdc = GetWindowDC(this->_baseMsg.hwnd());
		// OpenThemeData is wide-only (uxtheme.dll has no ANSI surface), so the
		// class-name string must be a wchar_t literal regardless of build mode.
		HTHEME hTheme = OpenThemeData(this->_baseMsg.hwnd(), L"LISTVIEW"); // borrow style from listview
		if (hTheme) {
			SetRect(&rc2, rc.left, rc.top, rc.left + 2, rc.bottom); // draw only the borders to avoid flickering
			DrawThemeBackground(hTheme, hdc, LVP_LISTGROUP, 0, &rc, &rc2); // draw themed left border
			SetRect(&rc2, rc.left, rc.top, rc.right, rc.top + 2);
			DrawThemeBackground(hTheme, hdc, LVP_LISTGROUP, 0, &rc, &rc2); // draw themed top border
			SetRect(&rc2, rc.right - 2, rc.top, rc.right, rc.bottom);
			DrawThemeBackground(hTheme, hdc, LVP_LISTGROUP, 0, &rc, &rc2); // draw themed right border
			SetRect(&rc2, rc.left, rc.bottom - 2, rc.right, rc.bottom);
			DrawThemeBackground(hTheme, hdc, LVP_LISTGROUP, 0, &rc, &rc2); // draw themed bottom border

			CloseThemeData(hTheme);
		}
		ReleaseDC(this->_baseMsg.hwnd(), hdc);
#endif
	}
};

}//namespace _wli
}//namespace wl
