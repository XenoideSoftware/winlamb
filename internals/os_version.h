/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <Windows.h>

namespace wl {
namespace _wli {

// Retrieves if the current operating system is Windows Vista or greater.
// Safe to execute in Windows 9x environments.
inline bool IsWindowsVistaOrGreater() noexcept {
    OSVERSIONINFOA osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

    // GetVersionExA is available on Windows 95/98.
    // GetVersionExW can return ERROR_CALL_NOT_IMPLEMENTED on Windows 9x.
#pragma warning(push)
#pragma warning(disable: 4996) // 'GetVersionExA': was declared deprecated
    if (GetVersionExA(&osvi)) {
        return (osvi.dwMajorVersion >= 6);
    }
#pragma warning(pop)

    return false;
}

// Retrieves if the current operating system is Windows 10 or greater.
// Safe to execute in Windows 9x environments.
inline bool IsWindows10OrGreater() noexcept {
    OSVERSIONINFOA osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

    // GetVersionExA is available on Windows 95/98.
    // GetVersionExW can return ERROR_CALL_NOT_IMPLEMENTED on Windows 9x.
#pragma warning(push)
#pragma warning(disable: 4996) // 'GetVersionExA': was declared deprecated
    if (GetVersionExA(&osvi)) {
        return (osvi.dwMajorVersion >= 10);
    }
#pragma warning(pop)

    return false;
}

} // namespace _wli
} // namespace wl
