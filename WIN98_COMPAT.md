# Win98 Compatibility Guide

This document describes how Win32 API calls in winlamb are guarded for
Windows 98 compatibility (`WINVER=0x0410`, `_WIN32_WINNT=0x0410`).

When adding new Win32 API calls inside winlamb, follow the patterns below.
Verify compatibility by running `cmake --build build-win98` from the repo root
(see the repo GEMINI.md / CLAUDE.md for toolchain setup).

---

## GetWindowLongPtr / SetWindowLongPtr

Only available on Win2000+ (`_WIN32_WINNT >= 0x0500`). On 32-bit Win9x,
`LONG` and `LONG_PTR` are the same width, so the non-Ptr variants are equivalent.

```cpp
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0500
    GetWindowLongPtrW(hWnd, GWLP_USERDATA);    // GWLP_* constants
    SetWindowLongPtrW(hWnd, GWLP_HINSTANCE, ...);
    SetWindowLongPtrW(hDlg, DWLP_USER, ...);   // dialogs
    SetWindowLongPtrW(hWnd, GWLP_ID, id);      // control ID
#else
    GetWindowLongW(hWnd, GWL_USERDATA);         // GWL_* fallback
    SetWindowLongW(hWnd, GWL_HINSTANCE, ...);
    SetWindowLongW(hDlg, DWL_USER, ...);        // dialogs
    SetWindowLongW(hWnd, GWL_ID, id);           // control ID
#endif
```

## GetClassLongPtr

Same threshold (`_WIN32_WINNT >= 0x0500`). Fallback to `GetClassLongW` + `GCL_*`:

```cpp
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0500
    GetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND);
#else
    GetClassLongW(hWnd, GCL_HBRBACKGROUND);
#endif
```

## VersionHelpers.h / IsWindowsVistaOrGreater()

`<VersionHelpers.h>` is a Vista-era SDK header — not available on Win9x builds.
Use the runtime helper in `internals/os_version.h` instead:

```cpp
#include "internals/os_version.h"
if (_wli::IsWindowsVistaOrGreater()) { ... }  // uses GetVersionExA internally
if (_wli::IsWindows10OrGreater()) { ... }
```

## ITaskbarList3 / progress_taskbar.h

Requires Win7 (`_WIN32_WINNT >= 0x0601`). The entire class is excluded below that:

```cpp
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0601
// #include <ShObjIdl.h> and class progress_taskbar live here
#endif
```

## CSIDL_PROGRAM_FILESX86

Constant not defined on Win9x SDKs. Guard with:

```cpp
#if defined(CSIDL_PROGRAM_FILESX86)
static std::wstring program_files_x86() { ... }
#endif
```

## Already correctly guarded (do not remove these guards)

- `SetWindowSubclass` / `RemoveWindowSubclass` / `DefSubclassProc` → `_WIN32_WINNT >= 0x0501`
- UxTheme (`IsThemeActive`, `OpenThemeData`, `DrawThemeBackground`) → `_WIN32_WINNT >= 0x0501`
- `WS_EX_NOREDIRECTIONBITMAP` → `WINVER >= 0x0602`
- Various `wmn::` / `wm::` struct members → per-version guards already in place
