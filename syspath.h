/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include "com.h"
#include "str.h"
#include <ShlObj.h>
#include "internals/tstring.h"

namespace wl {

// Retrieves system paths.
struct syspath final {
private:
	syspath() = delete;

public:
	static wl::tstring my_documents()      { return _get_shell_folder(CSIDL_MYDOCUMENTS); }
	static wl::tstring my_music()          { return _get_shell_folder(CSIDL_MYMUSIC); }
	static wl::tstring my_pictures()       { return _get_shell_folder(CSIDL_MYPICTURES); }
	static wl::tstring my_videos()         { return _get_shell_folder(CSIDL_MYVIDEO); }
	static wl::tstring desktop()           { return _get_shell_folder(CSIDL_DESKTOPDIRECTORY); }
	static wl::tstring app_data()          { return _get_shell_folder(CSIDL_APPDATA); }
	static wl::tstring app_data_local()    { return _get_shell_folder(CSIDL_LOCAL_APPDATA); }
	static wl::tstring app_data_common()   { return _get_shell_folder(CSIDL_COMMON_APPDATA); }
	static wl::tstring program_files()     { return _get_shell_folder(CSIDL_PROGRAM_FILES); }
#if defined(CSIDL_PROGRAM_FILESX86)
	static wl::tstring program_files_x86() { return _get_shell_folder(CSIDL_PROGRAM_FILESX86); }
#endif

	static wl::tstring temp() {
		TCHAR buf[MAX_PATH + 1]{};
		if (!GetTempPath(ARRAYSIZE(buf), buf)) { // will have trailing backslash here
			throw std::system_error(GetLastError(), std::system_category(),
				"GetTempPath failed");
		}
		wl::tstring ret = buf;
		ret.resize(ret.length() - 1); // remove trailing backslash
		return ret;
	}

private:
	static wl::tstring _get_shell_folder(int clsId) {
		TCHAR buf[MAX_PATH + 1]{};
		com::check_hr(
			SHGetFolderPath(nullptr, clsId, nullptr, 0, buf), // won't have trailing backslash
			"SHGetFolderPath failed");
		return buf;
	}
};

}//namespace wl
