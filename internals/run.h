/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once

#if defined(_MSC_VER)
#include <crtdbg.h>
#endif

#include <Windows.h>
#include <CommCtrl.h>
#include "lippincott.h"

#if defined(_MSC_VER)
#pragma comment(lib, "Comctl32.lib")
#endif

namespace wl {
namespace _wli {

template<typename wnd_mainT>
int run_main(HINSTANCE hInst, int cmdShow) noexcept {
	int ret = 0;
	try { // any exception which was not caught, except those from within message lambdas
		wnd_mainT wndMain;
		ret = wndMain.winmain_run(hInst, cmdShow);
	} catch (...) {
		lippincott();
		ret = -1;
	}
#if defined(_MSC_VER)
	_ASSERT(!_CrtDumpMemoryLeaks());
#endif
	return ret;
}

}//namespace _wli
}//namespace wl

// Instantiates your main class into a generic WinMain function.
#if defined(UNICODE) || defined(_UNICODE)
#define RUN(wnd_mainT) \
int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int cmdShow) { \
	return wl::_wli::run_main<wnd_mainT>(hInst, cmdShow); \
}
#else
#define RUN(wnd_mainT) \
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int cmdShow) { \
	return wl::_wli::run_main<wnd_mainT>(hInst, cmdShow); \
}
#endif
