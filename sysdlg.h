/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <algorithm>
#include <system_error>
#include "com.h"
#include "str.h"
#include "wnd.h"
#include <Shlobj.h>
#include "internals/compat.h"
#include "internals/sysdlg_priv.h"
#include <tchar.h>
#include "internals/tstring.h"

namespace wl {

// Wrappers to system dialogs.
namespace sysdlg {

// Ordinary MessageBox, centered at parent.
inline int msgbox(HWND hParent, const wl::tstring& title, const wl::tstring& text, UINT uType = 0) {
	if (hParent) { // the global hook will center the messagebox window on parent
		_wli::sysdlg_priv::hWndParent.val = hParent;
		_wli::sysdlg_priv::hHookMsgBox.val = SetWindowsHookEx(WH_CBT,
			&_wli::sysdlg_priv::msgbox_cbt_proc, nullptr, GetCurrentThreadId());

		if (!_wli::sysdlg_priv::hHookMsgBox.val) {
			throw std::system_error(GetLastError(), std::system_category(),
				"SetWindowsHookEx failed for message box");
		}
	}
	return MessageBox(hParent, text.c_str(), title.c_str(), uType);
}

// Ordinary MessageBox, centered at parent.
inline int msgbox(const wnd* parent, const wl::tstring& title, const wl::tstring& text, UINT uType = 0) {
	return msgbox(parent->hwnd(), title, text, uType);
}

// System dialog to select one file to be opened.
inline bool open_file(HWND hParent, const TCHAR* filterWithPipes, wl::tstring& buf) {
	OPENFILENAME         ofn{};
	TCHAR              tmpBuf[MAX_PATH]{};
	std::vector<TCHAR> zfilter = _wli::sysdlg_priv::format_file_filter(filterWithPipes);

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner   = hParent;
	ofn.lpstrFilter = &zfilter[0];
	ofn.lpstrFile   = tmpBuf;
	ofn.nMaxFile    = ARRAYSIZE(tmpBuf);
	ofn.Flags       = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST;// | OFN_HIDEREADONLY;

	bool ret = GetOpenFileName(&ofn) != 0;
	if (ret) buf = tmpBuf;
	return ret;
}

// System dialog to select one file to be opened.
inline bool open_file(const wnd* parent, const TCHAR* filterWithPipes, wl::tstring& buf) {
	return open_file(parent->hwnd(), filterWithPipes, buf);
}

// System dialog to select many files to be opened.
inline bool open_files(HWND hParent, const TCHAR* filterWithPipes, std::vector<wl::tstring>& arrBuf) {
	OPENFILENAME         ofn{};
	std::vector<TCHAR> multiBuf(65536, _T('\0')); // http://www.askjf.com/?q=2179s http://www.askjf.com/?q=2181s
	std::vector<TCHAR> zfilter = _wli::sysdlg_priv::format_file_filter(filterWithPipes);
	arrBuf.clear();

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner   = hParent;
	ofn.lpstrFilter = &zfilter[0];
	ofn.lpstrFile   = &multiBuf[0];
	ofn.nMaxFile    = static_cast<DWORD>(multiBuf.size()); // including terminating null
	ofn.Flags       = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_ENABLESIZING;
	//ofn.FlagsEx = OFN_EX_NOPLACESBAR;
	// Call to GetOpenFileName() causes "First-chance exception (KernelBase.dll): The RPC server is unavailable."
	// in debug mode, but nothing else happens. The only way to get rid of it was using OFN_EX_NOPLACESBAR flag,
	// don't know why!

	if (!GetOpenFileName(&ofn)) {
		DWORD err = CommDlgExtendedError();
		if (err == FNERR_BUFFERTOOSMALL) {
			throw std::system_error(err, std::system_category(),
				"GetOpenFileName failed, buffer is too small");
		}
		return false;
	}

	std::vector<wl::tstring> strs = str::split_multi_zero(&multiBuf[0]);
	if (strs.empty()) {
		throw std::runtime_error("GetOpenFileName didn't return multiple strings.");
	}

	if (strs.size() == 1) { // if user selected only 1 file, the string is the full path, and that's all
		arrBuf.emplace_back(strs[0]);
	} else { // user selected 2 or more files
		wl::tstring& basePath = strs[0]; // 1st string is the base path; others are the filenames
		arrBuf.resize(strs.size() - 1); // alloc return buffer

		for (size_t i = 0; i < strs.size() - 1; ++i) {
			arrBuf[i].reserve(basePath.length() + strs[i + 1].size() + 1); // room for backslash
			arrBuf[i] = basePath;
			arrBuf[i].append(_T("\\")).append(strs[i + 1]); // concat folder + file
		}
		std::sort(arrBuf.begin(), arrBuf.end());
	}
	return true;
}

// System dialog to select many files to be opened.
inline bool open_files(const wnd* parent, const TCHAR* filterWithPipes, std::vector<wl::tstring>& arrBuf) {
	return open_files(parent->hwnd(), filterWithPipes, arrBuf);
}

// System dialog to select where one file will be saved.
inline bool save_file(HWND hParent, const TCHAR* filterWithPipes, wl::tstring& buf, const wl::tstring& defFile) {
	OPENFILENAME         ofn{};
	TCHAR              tmpBuf[MAX_PATH]{};
	std::vector<TCHAR> zfilter = _wli::sysdlg_priv::format_file_filter(filterWithPipes);

	if (!defFile.empty()) lstrcpy(tmpBuf, defFile.c_str());

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner   = hParent;
	ofn.lpstrFilter = &zfilter[0];
	ofn.lpstrFile   = tmpBuf;
	ofn.nMaxFile    = ARRAYSIZE(tmpBuf);
	ofn.Flags       = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = _T("txt"); // apparently could be anything, will just force append of combo selected extension

	bool ret = GetSaveFileName(&ofn) != 0;
	if (ret) buf = tmpBuf;
	return ret;
}

// System dialog to select where one file will be saved.
inline bool save_file(const wnd* parent, const TCHAR* filterWithPipes, wl::tstring& buf, const wl::tstring& defFile) {
	return save_file(parent->hwnd(), filterWithPipes, buf, defFile);
}

// System dialog to select one folder.
inline bool choose_folder(HWND hParent, wl::tstring& buf) {
	com::lib comLib{com::lib::init::NOW};
	//LPITEMIDLIST pidlRoot = 0;
	//if (defFolder) SHParseDisplayName(defFolder, nullptr, &pidlRoot, 0, nullptr);

	BROWSEINFO bi{};
	bi.hwndOwner = hParent;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	PIDLIST_ABSOLUTE pidl = SHBrowseForFolder(&bi);
	if (!pidl) return false; // user cancelled

	TCHAR tmpbuf[MAX_PATH]{};
	if (!SHGetPathFromIDList(pidl, tmpbuf)) {
		throw std::runtime_error("SHGetPathFromIDList failed.");
	}

	buf = tmpbuf;
	return true;
}

// System dialog to select one folder.
inline bool choose_folder(const wnd* parent, wl::tstring& buf) {
	return choose_folder(parent->hwnd(), buf);
}

}//namespace sysdlg
}//namespace wl
