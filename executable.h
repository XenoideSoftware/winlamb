/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include "com.h"
#include "str.h"
#include <tchar.h>
#include "internals/tstring.h"

namespace wl {

// Executable-related utilities.
namespace executable {

// Retrieves path to current running EXE itself.
inline wl::tstring get_own_path() {
	TCHAR buf[MAX_PATH + 1]{};
	if (!GetModuleFileName(nullptr, buf, ARRAYSIZE(buf))) { // full path name
		throw std::system_error(GetLastError(), std::system_category(),
			"GetModuleFileName failed");
	}
	wl::tstring ret = buf;
	ret.resize(ret.find_last_of(_T('\\'))); // truncate removing EXE filename and trailing backslash
#ifdef _DEBUG
	ret.resize(ret.find_last_of(_T('\\'))); // bypass "Debug" folder, remove trailing backslash too
#ifdef _WIN64
	ret.resize(ret.find_last_of(_T('\\'))); // bypass "x64" folder, remove trailing backslash again
#endif
#endif
	return ret;
}

// Retrieves the program's command line, tokenized.
inline std::vector<wl::tstring> get_cmd_line() {
	return str::split_quoted(GetCommandLine());
}

// Synchronous execution of a command line in a separated process.
inline DWORD exec(const wl::tstring& cmdLine) {
	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	STARTUPINFO si{};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;

	PROCESS_INFORMATION pi{};
	DWORD dwExitCode = 1; // returned by executed program

	wl::tstring cmdLine2 = cmdLine; // http://blogs.msdn.com/b/oldnewthing/archive/2009/06/01/9673254.aspx

	if (!CreateProcess(nullptr, &cmdLine2[0], &sa, nullptr, FALSE,
		0, nullptr, nullptr, &si, &pi))
	{
		throw std::system_error(GetLastError(), std::system_category(),
			"CreateProcess failed");
	}

	WaitForSingleObject(pi.hProcess, INFINITE); // the program flow is stopped here to wait
	GetExitCodeProcess(pi.hProcess, &dwExitCode);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return dwExitCode;
}

// Runs the shell-associated program to the given file, for example Notepad to a TXT.
inline void shell_exec(const wl::tstring& file, INT showCmd = SW_SHOWNORMAL) {
	com::lib comLib{com::lib::init::NOW};
	int h = static_cast<int>(reinterpret_cast<INT_PTR>(ShellExecute(nullptr,
		_T("open"), file.c_str(), nullptr, nullptr, showCmd)));
	if (h <= 8) {
		throw std::system_error(h, std::system_category(),
			"ShellExecute failed");
	} else if (h <= 32) {
		throw std::runtime_error(str::to_ascii(
			str::format(_T("ShellExecute failed: error %d."), h) ));
	}
}

}//namespace executable
}//namespace wl
