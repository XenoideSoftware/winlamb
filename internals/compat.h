/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once

// Compatibility shim for older Windows SDK headers (TDM-GCC, MinGW pre-w64,
// pre-XP Platform SDK, etc). Provides polyfills for symbols, macros and
// structs that newer SDKs ship in <winnt.h>, <tchar.h>, <ShTypes.h>,
// <commctrl.h>, <commoncontrols.h> and <winuser.h> but are missing on older
// toolchains used to target legacy Windows.

#include <Windows.h>
#include <CommCtrl.h>
#include <ShlObj.h>
#include <tchar.h>
#include <ctype.h>
#include <wchar.h>
#include <stdio.h>
// std::reference_wrapper is forward-declared by libstdc++ but only completed
// by <functional>. Several winlamb collections embed reference_wrapper<HWND>
// fields, so pull the definition in centrally to avoid per-file fixups.
#include <functional>

// ---------------------------------------------------------------- ARRAYSIZE
#ifndef ARRAYSIZE
#define ARRAYSIZE(A) (sizeof(A) / sizeof((A)[0]))
#endif

// ------------------------------------------------------------ SecureZeroMemory
#ifndef SecureZeroMemory
#define SecureZeroMemory(ptr, sz) ::ZeroMemory((ptr), (sz))
#endif

// ---------------------------------------------------------- PIDLIST_ABSOLUTE
// Defined in newer <ShTypes.h>; older SDKs only expose LPITEMIDLIST.
#if !defined(PIDLIST_ABSOLUTE) && !defined(_PIDLIST_ABSOLUTE_DEFINED)
typedef LPITEMIDLIST PIDLIST_ABSOLUTE;
typedef LPCITEMIDLIST PCIDLIST_ABSOLUTE;
#define _PIDLIST_ABSOLUTE_DEFINED 1
#endif

// ------------------------------------------------------------- SHIL_* values
// Image-list resolution selectors used with SHGetImageList. Defined in
// <commoncontrols.h>, which is missing on older toolchains.
#ifndef SHIL_LARGE
#define SHIL_LARGE        0
#endif
#ifndef SHIL_SMALL
#define SHIL_SMALL        1
#endif
#ifndef SHIL_EXTRALARGE
#define SHIL_EXTRALARGE   2
#endif
#ifndef SHIL_SYSSMALL
#define SHIL_SYSSMALL     3
#endif
#ifndef SHIL_JUMBO
#define SHIL_JUMBO        4
#endif
#ifndef SHIL_LAST
#define SHIL_LAST         SHIL_JUMBO
#endif

// ----------------------------------------------------------- IID_IImageList
// SHGetImageList expects an interface IID. We don't actually use the
// IImageList vtable (the result is cast to HIMAGELIST and consumed by the
// classic ImageList_* API), but the call site needs the GUID symbol.
#if !defined(__IImageList_INTERFACE_DEFINED__) && !defined(WL_IID_IIMAGELIST_DEFINED)
#define WL_IID_IIMAGELIST_DEFINED 1
namespace wl {
namespace _wli {
// {46EB5926-582E-4017-9FDF-E8998DAA0950}
static const GUID IID_IImageList_polyfill =
    { 0x46EB5926, 0x582E, 0x4017, { 0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x09, 0x50 } };
} // namespace _wli
} // namespace wl
// Forward declaration so existing code that types `IImageList*` still compiles;
// only used as an opaque pointer that we cast to HIMAGELIST.
struct IImageList;
#define IID_IImageList (::wl::_wli::IID_IImageList_polyfill)
#endif

// ---------------------------------------------------- SHGetImageList prototype
// Declared in <shlobj.h> behind XP+ guards. TDM-GCC's SDK omits it entirely;
// the export still exists in shell32.dll on XP and later, so a manual
// declaration lets the linker resolve it.
#if !defined(SHGetImageList) && !defined(WL_SHGETIMAGELIST_DECLARED)
#define WL_SHGETIMAGELIST_DECLARED 1
extern "C" {
WINSHELLAPI HRESULT WINAPI SHGetImageList(int iImageList, REFIID riid, void** ppvObj);
}
#endif

// =====================================================================
// Legacy commctrl/winuser polyfill block
//
// TDM-GCC ships with a frozen pre-XP MinGW SDK, so even bumping
// _WIN32_WINNT to 0x0501 doesn't expose the XP-era additions to
// <commctrl.h> and <winuser.h>. We detect that environment via a sentinel
// constant (LVS_EX_DOUBLEBUFFER, added in XP and present in every modern
// SDK) and define the missing symbols ourselves with their canonical
// values from the public Windows SDK.
// =====================================================================
#ifndef LVS_EX_DOUBLEBUFFER

// ---- Listview view modes (LVM_SETVIEW / LVM_GETVIEW selectors)
#define LV_VIEW_ICON      0x0000
#define LV_VIEW_DETAILS   0x0001
#define LV_VIEW_SMALLICON 0x0002
#define LV_VIEW_LIST      0x0003
#define LV_VIEW_TILE      0x0004

// ---- Listview / treeview extended styles
#define LVS_EX_DOUBLEBUFFER 0x00010000
#ifndef TVS_EX_DOUBLEBUFFER
#define TVS_EX_DOUBLEBUFFER 0x0004
#endif

// ---- Listview LVM messages added in XP (constants from commctrl.h spec)
#ifndef LVM_GETITEMINDEXRECT
#define LVM_GETITEMINDEXRECT (LVM_FIRST + 209)
#endif
#ifndef LVM_ISITEMVISIBLE
#define LVM_ISITEMVISIBLE    (LVM_FIRST + 182)
#endif

// ---- LVITEMINDEX struct (XP+)
typedef struct tagLVITEMINDEX {
    int iItem;
    int iGroup;
} LVITEMINDEX, *PLVITEMINDEX;

// ---- ListView_* macro polyfills using the LVM messages above.
// The canonical Win32 macro packs iColumn/rectType into the RECT struct
// before sending LVM_GETITEMINDEXRECT — replicate that behavior here.
#ifndef ListView_GetItemIndexRect
#define ListView_GetItemIndexRect(hwnd, plvii, iColumn, rectType, prc) \
    ((BOOL)((prc) ? ((prc)->top = (iColumn), (prc)->left = (rectType)) : 0), \
     (BOOL)SNDMSG((hwnd), LVM_GETITEMINDEXRECT, \
        (WPARAM)(LVITEMINDEX*)(plvii), (LPARAM)(RECT*)(prc)))
#endif

#ifndef ListView_IsItemVisible
#define ListView_IsItemVisible(hwnd, i) \
    ((BOOL)SNDMSG((hwnd), LVM_ISITEMVISIBLE, (WPARAM)(i), 0))
#endif

// ---- TreeView_GetItemState polyfill (XP+ macro). The TVITEMEX struct
// itself is not aliased here because some legacy SDKs (e.g. TDM-GCC's
// commctrl.h) already declare it with a slightly different layout, which
// would collide. winlamb's treeview helpers use plain TVITEM, which is a
// strict subset of TVITEMEX and is available everywhere.
#ifndef TreeView_GetItemState
#define TreeView_GetItemState(hwndTV, hti, mask) \
    ((UINT)SNDMSG((hwndTV), TVM_GETITEMSTATE, (WPARAM)(HTREEITEM)(hti), (LPARAM)(mask)))
#endif

// ---- Statusbar additions
#ifndef SBARS_TOOLTIPS
#define SBARS_TOOLTIPS 0x0800
#endif
#ifndef SB_SETICON
#define SB_SETICON (WM_USER + 15)
#endif

// ---- Progressbar marquee additions
#ifndef PBS_MARQUEE
#define PBS_MARQUEE 0x08
#endif
#ifndef PBS_SMOOTHREVERSE
#define PBS_SMOOTHREVERSE 0x10
#endif
#ifndef PBM_SETMARQUEE
#define PBM_SETMARQUEE (WM_USER + 10)
#endif

// ---- WM_INPUTLANGCHANGE wParam flags
#ifndef INPUTLANGCHANGE_SYSCHARSET
#define INPUTLANGCHANGE_SYSCHARSET 0x0001
#endif
#ifndef INPUTLANGCHANGE_FORWARD
#define INPUTLANGCHANGE_FORWARD    0x0002
#endif
#ifndef INPUTLANGCHANGE_BACKWARD
#define INPUTLANGCHANGE_BACKWARD   0x0004
#endif

// ---- WM_QUERYENDSESSION lParam flags
#ifndef ENDSESSION_CLOSEAPP
#define ENDSESSION_CLOSEAPP 0x00000001
#endif
#ifndef ENDSESSION_CRITICAL
#define ENDSESSION_CRITICAL 0x40000000
#endif

// ---- WM_POWERBROADCAST additions
#ifndef PBT_POWERSETTINGCHANGE
#define PBT_POWERSETTINGCHANGE 0x8013
#endif
typedef struct {
    GUID  PowerSetting;
    DWORD DataLength;
    UCHAR Data[1];
} POWERBROADCAST_SETTING, *PPOWERBROADCAST_SETTING;

// ---- SHFILEOPSTRUCT FOF_NO_UI flag (XP+; combination of older flags)
#ifndef FOF_NO_UI
#define FOF_NO_UI (FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR)
#endif

// ---- CSIDL_MYDOCUMENTS (XP+; alias for CSIDL_PERSONAL on older SDKs)
#ifndef CSIDL_MYDOCUMENTS
#define CSIDL_MYDOCUMENTS CSIDL_PERSONAL
#endif

// ---- VsStyle.h LISTVIEW theme parts
// base_user_ctrl.h pulls VsStyle.h to get LVP_LISTGROUP for DrawThemeBackground.
// TDM-GCC's SDK omits VsStyle.h entirely; the canonical SDK value is 2
// (LISTVIEWPARTS::LVP_LISTGROUP, with LVP_LISTITEM = 1).
#ifndef LVP_LISTGROUP
#define LVP_LISTGROUP 2
#endif

// ---- Notification structs added in XP/IE6
// Only the fields actually accessed elsewhere in winlamb need to match the
// real layout; winlamb's wmn::* wrappers just reinterpret_cast lParam, so
// declaring the type is enough to keep the code well-formed.
typedef struct tagNMDATETIMEFORMATA {
    NMHDR  nmhdr;
    LPCSTR pszFormat;
    SYSTEMTIME st;
    LPCSTR pszDisplay;
    CHAR   szDisplay[64];
} NMDATETIMEFORMATA, *LPNMDATETIMEFORMATA;
typedef struct tagNMDATETIMEFORMATW {
    NMHDR   nmhdr;
    LPCWSTR pszFormat;
    SYSTEMTIME st;
    LPCWSTR pszDisplay;
    WCHAR   szDisplay[64];
} NMDATETIMEFORMATW, *LPNMDATETIMEFORMATW;
#ifdef UNICODE
typedef NMDATETIMEFORMATW  NMDATETIMEFORMAT;
typedef LPNMDATETIMEFORMATW LPNMDATETIMEFORMAT;
#else
typedef NMDATETIMEFORMATA  NMDATETIMEFORMAT;
typedef LPNMDATETIMEFORMATA LPNMDATETIMEFORMAT;
#endif

typedef struct tagNMDATETIMESTRINGA {
    NMHDR  nmhdr;
    LPCSTR pszUserString;
    SYSTEMTIME st;
    DWORD  dwFlags;
} NMDATETIMESTRINGA, *LPNMDATETIMESTRINGA;
typedef struct tagNMDATETIMESTRINGW {
    NMHDR   nmhdr;
    LPCWSTR pszUserString;
    SYSTEMTIME st;
    DWORD   dwFlags;
} NMDATETIMESTRINGW, *LPNMDATETIMESTRINGW;
#ifdef UNICODE
typedef NMDATETIMESTRINGW  NMDATETIMESTRING;
typedef LPNMDATETIMESTRINGW LPNMDATETIMESTRING;
#else
typedef NMDATETIMESTRINGA  NMDATETIMESTRING;
typedef LPNMDATETIMESTRINGA LPNMDATETIMESTRING;
#endif

typedef struct tagNMDATETIMEWMKEYDOWNA {
    NMHDR  nmhdr;
    int    nVirtKey;
    LPCSTR pszFormat;
    SYSTEMTIME st;
} NMDATETIMEWMKEYDOWNA, *LPNMDATETIMEWMKEYDOWNA;
typedef struct tagNMDATETIMEWMKEYDOWNW {
    NMHDR   nmhdr;
    int     nVirtKey;
    LPCWSTR pszFormat;
    SYSTEMTIME st;
} NMDATETIMEWMKEYDOWNW, *LPNMDATETIMEWMKEYDOWNW;
#ifdef UNICODE
typedef NMDATETIMEWMKEYDOWNW  NMDATETIMEWMKEYDOWN;
typedef LPNMDATETIMEWMKEYDOWNW LPNMDATETIMEWMKEYDOWN;
#else
typedef NMDATETIMEWMKEYDOWNA  NMDATETIMEWMKEYDOWN;
typedef LPNMDATETIMEWMKEYDOWNA LPNMDATETIMEWMKEYDOWN;
#endif

typedef struct tagNMOBJECTNOTIFY {
    NMHDR   hdr;
    int     iItem;
    const IID* piid;
    void*   pObject;
    HRESULT hResult;
    DWORD   dwFlags;
} NMOBJECTNOTIFY, *LPNMOBJECTNOTIFY;

typedef struct tagNMTTCUSTOMDRAW {
    NMCUSTOMDRAW nmcd;
    UINT         uDrawFlags;
} NMTTCUSTOMDRAW, *LPNMTTCUSTOMDRAW;

typedef struct tagTVKEYDOWN {
    NMHDR hdr;
    WORD  wVKey;
    UINT  flags;
} NMTVKEYDOWN, *LPNMTVKEYDOWN;

#endif // legacy commctrl polyfill block

// =====================================================================
// UxTheme polyfill — declares HTHEME / theme APIs when <Uxtheme.h> is
// unavailable. Their entry points live in uxtheme.dll (XP+); on Win9x
// the callers in base_user_ctrl.h already early-out via IsThemeActive(),
// so the imports are never resolved at runtime there.
// =====================================================================
#if defined(__has_include)
    #if !__has_include(<Uxtheme.h>)
        #define WL_NEED_UXTHEME_POLYFILL 1
    #endif
#endif

#ifdef WL_NEED_UXTHEME_POLYFILL
DECLARE_HANDLE(HTHEME);
extern "C" {
WINAPI BOOL    IsThemeActive(void);
WINAPI BOOL    IsAppThemed(void);
WINAPI HTHEME  OpenThemeData(HWND hwnd, LPCWSTR pszClassList);
WINAPI HRESULT CloseThemeData(HTHEME hTheme);
WINAPI HRESULT DrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId,
    int iStateId, const RECT* pRect, const RECT* pClipRect);
}
#endif

// =====================================================================
// IID_PPV_ARGS polyfill — needed by com_ptr.h's templated overloads.
// The standard SDK macro relies on __uuidof, which GCC supports only
// with -fms-extensions. We supply the same expansion when the macro is
// missing; if the host TU lacks -fms-extensions the resulting compile
// error points users at com_ptr's explicit IID overloads instead.
// =====================================================================
#ifndef IID_PPV_ARGS
#define IID_PPV_ARGS(ppType) \
    __uuidof(**(ppType)), reinterpret_cast<void**>(ppType)
#endif

// =====================================================================
// MsXml2 availability flag — xml.h pulls <MsXml2.h> for IXMLDOMDocument2
// and friends, which TDM-GCC's SDK doesn't ship. Forward-declarations
// alone aren't enough because wl::com::ptr<T>::~ptr() invokes T->Release(),
// which requires a complete type with a vtable. Detect the missing header
// here and let xml.h gate its entire class behind WL_NO_MSXML2 instead.
// =====================================================================
#if defined(__has_include)
    #if !__has_include(<MsXml2.h>)
        #define WL_NO_MSXML2 1
    #endif
#endif

namespace wl {
namespace _wli {
namespace compat {

// -------------------------------------------------------------- _istblank
// Old MinGW <tchar.h> doesn't expose _istblank. Provide a TCHAR-aware version.
inline bool istblank(TCHAR ch) noexcept {
    return ch == _T(' ') || ch == _T('\t');
}

// -------------------------------------------------------- tcsnicmp polyfill
// Old MinGW <tchar.h> maps _tcsnicmp -> _strnicmp / _wcsnicmp, but those
// CRT names are missing in TDM-GCC builds. Provide a portable wrapper
// using CompareString, which is available on every Windows version.
inline int tcsnicmp(const TCHAR* a, const TCHAR* b, size_t n) noexcept {
    if (n == 0) return 0;
    int cmp = CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE,
        a, static_cast<int>(n), b, static_cast<int>(n));
    // CSTR_LESS_THAN=1, CSTR_EQUAL=2, CSTR_GREATER_THAN=3
    return cmp - CSTR_EQUAL;
}

// ------------------------------------------------------------ itot polyfill
// _itot / _itoa are not exported by every CRT (notably TDM-GCC). wsprintf
// is part of user32 and uniformly available.
inline void itot(int value, TCHAR* buffer, int /*radix*/) noexcept {
    // Caller always passes radix 10 in this codebase, so we hardcode %d.
    wsprintf(buffer, _T("%d"), value);
}

} // namespace compat
} // namespace _wli
} // namespace wl
