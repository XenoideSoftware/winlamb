/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <system_error>
#include "internals/os_version.h"
#include "wnd.h"
#include "internals/enable_bitmask_operators.h"

namespace wl {

// Wrapper to HFONT handle.
class font final {
private:
	HFONT _hFont = nullptr;

public:
	// Can be combined with bitmask operators.
	enum class deco : BYTE {
		NONE      = 0b00000000,
		BOLD      = 0b00000001,
		ITALIC    = 0b00000010,
		STRIKEOUT = 0b00000100,
		UNDERLINE = 0b00001000
	};

	~font() {
		this->destroy();
	}

	font() = default;

	font(font&& other) noexcept : _hFont{other._hFont} {
		other._hFont = nullptr;
	}

	HFONT hfont() const noexcept {
		return this->_hFont;
	}

	font& operator=(font&& other) noexcept {
		this->destroy();
		std::swap(this->_hFont, other._hFont);
		return *this;
	}

	font& destroy() noexcept {
		if (this->_hFont) {
			DeleteObject(this->_hFont);
			this->_hFont = nullptr;
		}
		return *this;
	}

	font& create(const LOGFONT& lf) {
		this->destroy();
		this->_hFont = CreateFontIndirect(&lf);
		if (!this->_hFont) {
			throw std::system_error(GetLastError(), std::system_category(),
				"CreateFontIndirect failed");
		}
		return *this;
	}

	font& create(const TCHAR* fontName, BYTE size, deco style = deco::NONE) {
		this->destroy();
		LOGFONT lf{};
		lstrcpy(lf.lfFaceName, fontName);
		lf.lfHeight = -(size + 3);

		auto hasDeco = [=](deco yourDeco) noexcept -> BOOL {
			return (static_cast<BYTE>(style) &
				static_cast<BYTE>(yourDeco)) != 0 ? TRUE : FALSE;
		};

		lf.lfWeight    = hasDeco(deco::BOLD) ? FW_BOLD : FW_DONTCARE;
		lf.lfItalic    = hasDeco(deco::ITALIC);
		lf.lfUnderline = hasDeco(deco::UNDERLINE);
		lf.lfStrikeOut = hasDeco(deco::STRIKEOUT);

		return this->create(lf);
	}

	// Sets the font on the given control.
	void set_on(wnd& control) const noexcept {
		SendMessage(control.hwnd(), WM_SETFONT,
			reinterpret_cast<WPARAM>(_hFont), TRUE);
	}

	// Sets the font on the given control.
	void set_on(std::initializer_list<std::reference_wrapper<wnd>> ctrls) const noexcept {
		for (wnd& ctrl : ctrls) {
			this->set_on(ctrl);
		}
	}

	// Create the same exact font used by UI, like Tahoma or Segoe UI.
	font& create_ui() {
		return this->create(util::get_ui());
	}

public:
	class util final {
	private:
		util() = delete;

	private:
		// Free CALLBACK functions used by the EnumChildWindows / EnumFontFamilies
		// helpers below — captureless lambdas decay to __cdecl on 32-bit MinGW,
		// which can't implicitly convert to the __stdcall typedefs the Win32
		// APIs require, so we hoist them into stand-alone CALLBACK-qualified
		// functions and tunnel state through the LPARAM as before.
		static BOOL CALLBACK _set_font_on_child(HWND hWnd, LPARAM lp) noexcept {
			SendMessage(hWnd, WM_SETFONT,
				reinterpret_cast<WPARAM>(reinterpret_cast<HFONT>(lp)),
				MAKELPARAM(FALSE, 0));
			return TRUE;
		}

		static int CALLBACK _font_exists_proc(const LOGFONT* /*lpelf*/,
			const TEXTMETRIC* /*lpntm*/, DWORD /*fontType*/, LPARAM lp) noexcept
		{
			bool* pIsInstalled = reinterpret_cast<bool*>(lp);
			*pIsInstalled = true; // if we're here, the font does exist
			return 0;
		}

	public:
		// Applies default UI font on all children of the window.
		static void set_ui_on_children(HWND hParent) {
			static font oneFont; // keep one single font instance for all windows
			if (!oneFont._hFont) oneFont.create_ui();

			SendMessage(hParent, WM_SETFONT,
				reinterpret_cast<WPARAM>(oneFont._hFont),
				MAKELPARAM(FALSE, 0));
			EnumChildWindows(hParent, &_set_font_on_child,
				reinterpret_cast<LPARAM>(oneFont._hFont));
		}

		// Checks if the font is currently installed.
		static bool exists(const TCHAR* fontName) {
			// http://cboard.cprogramming.com/windows-programming/90066-how-determine-if-font-support-unicode.html
			bool isInstalled = false;
			HDC hdc = GetDC(nullptr);
			if (!hdc) {
				throw std::system_error(GetLastError(), std::system_category(),
					"GetDC failed when checking if font exists");
			}
			EnumFontFamilies(hdc, fontName, &_font_exists_proc,
				reinterpret_cast<LPARAM>(&isInstalled));
			ReleaseDC(nullptr, hdc);
			return isInstalled;
		}

		// Gets the LOGFONT options used by UI, like Tahoma or Segoe UI.
		static LOGFONT get_ui() {
			NONCLIENTMETRICS ncm{};
			ncm.cbSize = sizeof(ncm);
			if (!IsWindowsVistaOrGreater()) {
				ncm.cbSize -= sizeof(ncm.iBorderWidth);
			}
			SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
			return ncm.lfMenuFont;
		}
	};
};

}//namespace wl

// `enable_bitmask_operators` is a global-scope primary template, so its
// specialization must also live at global scope (older GCC enforces this
// strictly; MSVC tolerated the wl::-scoped form).
ENABLE_BITMASK_OPERATORS(wl::font::deco);
