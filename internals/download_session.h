/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <system_error>
#include <utility>
#include <Windows.h>
#include "tstring.h"
// WinHTTP shipped with Windows 2000 and was bolted onto XP+. TDM-GCC's
// frozen MinGW SDK predates the header altogether. Skip the entire wrapper
// when <winhttp.h> isn't available — calling code that needs wl::download
// will surface a clean "type not declared" error instead of a header miss.
#if defined(__has_include)
    #if !__has_include(<winhttp.h>)
        #define WL_NO_WINHTTP 1
    #endif
#endif
#ifndef WL_NO_WINHTTP
#include <winhttp.h>
#include <tchar.h>
#pragma comment(lib, "Winhttp.lib")

namespace wl {
namespace _wli {

// Wrapper to HINTERNET handle.
class download_session final {
private:
	HINTERNET _hSession = nullptr;

public:
	~download_session() {
		this->close();
	}

	download_session() = default;
	download_session(download_session&& other) noexcept : _hSession{other._hSession} { other._hSession = nullptr; }

	HINTERNET hsession() const noexcept {
		return this->_hSession;
	}

	download_session& operator=(download_session&& other) noexcept {
		this->close();
		std::swap(this->_hSession, other._hSession);
		return *this;
	}

	void close() noexcept {
		if (this->_hSession) {
			WinHttpCloseHandle(this->_hSession);
			this->_hSession = nullptr;
		}
	}

	download_session& open(const TCHAR* userAgent = _T("WinLamb/1.0")) {
		if (!this->_hSession) {
			// http://social.msdn.microsoft.com/forums/en-US/vclanguage/thread/45ccd91c-6794-4f9b-8f4f-865c76cc146d
			if (!WinHttpCheckPlatform()) {
				throw std::system_error(GetLastError(), std::system_category(),
					"WinHttpCheckPlatform failed, this platform is not supported by WinHTTP");
			}

			// WinHttpOpen is wide-only (no ANSI variant); widen at the boundary.
			this->_hSession = WinHttpOpen(wl::to_wstring(userAgent).c_str(),
				WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
				WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (!this->_hSession) {
				throw std::system_error(GetLastError(), std::system_category(),
					"WinHttpOpen failed when opening session");
			}
		}

		return *this;
	}
};

}//namespace _wli
}//namespace wl

#endif // !WL_NO_WINHTTP
