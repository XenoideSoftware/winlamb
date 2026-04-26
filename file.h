/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <string>
#include <system_error>
#include <vector>
#include "datetime.h"
#include <Shellapi.h>
#include <tchar.h>
#include "internals/tstring.h"

namespace wl {

// Wrapper to a low-level HANDLE of a file.
class file final {
public:
	// File access type.
	enum class access { READONLY, READWRITE };

	// Date information of a file.
	struct dates final {
		datetime creation;
		datetime lastAccess;
		datetime lastWrite;
	};

private:
	HANDLE _hFile = nullptr;
	access _access = access::READONLY;
	size_t _sz = -1;

public:
	~file() {
		this->close();
	}

	file() = default;
	file(file&& other) noexcept { this->operator=(std::move(other)); }

	file& operator=(file&& other) noexcept {
		this->close();
		std::swap(this->_hFile, other._hFile);
		std::swap(this->_access, other._access);
		std::swap(this->_sz, other._sz);
		return *this;
	}

	// Returns the handle to the file.
	HANDLE hfile() const noexcept {
		return this->_hFile;
	}

	// Tells if the file was opened as read/write or read-only.
	access access_type() const noexcept {
		return this->_access;
	}

	// Closes the file, wrapper to CloseHandle.
	file& close() noexcept {
		if (this->_hFile) {
			CloseHandle(this->_hFile);
			this->_hFile = nullptr;
			this->_access = access::READONLY;
			this->_sz = -1; // http://stackoverflow.com/a/19483690
		}
		return *this;
	}

	// Retrieve the file size in bytes.
	size_t size() noexcept {
		if (this->_sz == -1) {
			this->_sz = GetFileSize(this->_hFile, nullptr); // cache
		}
		return this->_sz;
	}

private:
	file& _raw_open(const wl::tstring& filePath, DWORD desiredAccess,
		DWORD shareMode, DWORD creationDisposition)
	{
		if (filePath.empty()) {
			throw std::invalid_argument("No file path specified.");
		}

		this->close();
		bool isReadWrite = (desiredAccess & GENERIC_WRITE) != 0;

		this->_hFile = CreateFile(filePath.c_str(), desiredAccess, shareMode,
			nullptr, creationDisposition, 0, nullptr);
		if (this->_hFile == INVALID_HANDLE_VALUE) {
			this->_hFile = nullptr;
			throw std::system_error(GetLastError(), std::system_category(),
				isReadWrite ?
					"CreateFile failed to open file as read-write" :
					"CreateFile failed to open file as read-only");
		}

		this->_access = isReadWrite ?
			access::READWRITE : access::READONLY; // keep for future checks
		return *this;
	}

public:
	// Opens a file, throwing an exception if it doesn't exist.
	file& open_existing(const TCHAR* filePath, access accessType) {
		if (!util::exists(filePath)) {
			throw std::invalid_argument("File doesn't exist.");
		}
		return this->_raw_open(filePath,
			GENERIC_READ | (accessType == access::READWRITE ? GENERIC_WRITE : 0),
			(accessType == access::READWRITE) ? 0 : FILE_SHARE_READ,
			OPEN_EXISTING); // fails if file doesn't exist
	}

	// Opens a file, throwing an exception if it doesn't exist.
	file& open_existing(const wl::tstring& filePath, access accessType) {
		return this->open_existing(filePath.c_str(), accessType);
	}

	// Opens a file as read/write, creates if it doesn't exist.
	file& open_or_create(const TCHAR* filePath) {
		return this->_raw_open(filePath, GENERIC_READ | GENERIC_WRITE, 0, OPEN_ALWAYS);
	}

	// Opens a file as read/write, creates if it doesn't exist.
	file& open_or_create(const wl::tstring& filePath) {
		return this->open_or_create(filePath.c_str());
	}

private:
	void _check_file_opened() const {
		if (!this->_hFile) {
			throw std::logic_error("File has not been opened.");
		}
	}

	void _check_file_read_only() const {
		if (this->_access == access::READONLY) {
			throw std::logic_error("File was opened for read-only access.");
		}
	}

public:
	// Truncates or expands the file, according to the new size; zero will empty the file.
	file& set_new_size(size_t numBytes) {
		this->_check_file_opened();
		this->_check_file_read_only();
		if (this->size() == numBytes) return *this; // nothing to do

		auto tooBad = [this](DWORD err, const char* msg) -> void {
			this->close();
			throw std::system_error(err, std::system_category(), msg);
		};

		DWORD r = SetFilePointer(this->_hFile, static_cast<LONG>(numBytes), nullptr, FILE_BEGIN);
		if (r == INVALID_SET_FILE_POINTER) {
			tooBad(GetLastError(), "SetFilePointer failed when setting new file size");
		}

		if (!SetEndOfFile(this->_hFile)) {
			tooBad(GetLastError(), "SetEndOfFile failed when setting new file size");
		}

		r = SetFilePointer(this->_hFile, 0, nullptr, FILE_BEGIN); // rewind
		if (r == INVALID_SET_FILE_POINTER) {
			tooBad(GetLastError(), "SetFilePointer failed to rewind the file pointer when setting new file size");
		}

		this->_sz = numBytes; // update
		return *this;
	}

	// Calls SetFilePointer to set internal pointer to begin of the file.
	file& rewind() {
		this->_check_file_opened();
		if (SetFilePointer(this->_hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			throw std::system_error(GetLastError(), std::system_category(),
				"SetFilePointer failed to rewind the file");
		}
		return *this;
	}

	// Reads all file content at once into a buffer.
	file& read_to_buffer(std::vector<BYTE>& buf) {
		this->_check_file_opened();
		buf.resize(this->size());
		DWORD bytesRead = 0;
		if (!ReadFile(this->_hFile, &buf[0], static_cast<DWORD>(buf.size()), &bytesRead, nullptr)) {
			throw std::system_error(GetLastError(), std::system_category(),
				"ReadFile failed");
		}
		return *this;
	}

	// Retrieves all file content at once.
	std::vector<BYTE> read() {
		std::vector<BYTE> buf;
		this->read_to_buffer(buf);
		return buf;
	}

	// Writes content to file, wrapper to WriteFile.
	file& write(const BYTE* pData, size_t sz) {
		this->_check_file_opened();
		this->_check_file_read_only();

		// File boundary will be expanded if needed.
		// Internal file pointer will move forward.
		DWORD dwWritten = 0;
		if (!WriteFile(this->_hFile, pData, static_cast<DWORD>(sz), &dwWritten, nullptr)) {
			throw std::system_error(GetLastError(), std::system_category(),
				"WriteFile failed");
		}
		return *this;
	}

	// Writes content to file, wrapper to WriteFile.
	file& write(const std::vector<BYTE>& data) {
		return this->write(&data[0], data.size());
	}

	// Gets creation, last access and last write dates, wrapper to GetFileTime.
	dates get_dates() const {
		this->_check_file_opened();
		FILETIME ftCreation{}, ftLastAccess{}, ftLastWrite{};
		if (!GetFileTime(this->_hFile, &ftCreation, &ftLastAccess, &ftLastWrite)) {
			throw std::system_error(GetLastError(), std::system_category(),
				"GetFileTime failed");
		}
		return {ftCreation, ftLastAccess, ftLastWrite};
	}

public:
	// File utilities.
	class util final {
	private:
		util() = delete;

	public:
		// Reads all file content at once into a buffer.
		static void read_to_buffer(const TCHAR* filePath, std::vector<BYTE>& buf) {
			file fin;
			fin.open_existing(filePath, access::READONLY);
			fin.read_to_buffer(buf);
		}

		// Reads all file content at once into a buffer.
		static void read_to_buffer(const wl::tstring& filePath, std::vector<BYTE>& buf) {
			read_to_buffer(filePath.c_str(), buf);
		}

		// Retrieves all file content at once.
		static std::vector<BYTE> read(const TCHAR* filePath) {
			std::vector<BYTE> buf;
			read_to_buffer(filePath, buf);
			return buf;
		}

		// Retrieves all file content at once.
		static std::vector<BYTE> read(const wl::tstring& filePath) {
			return read(filePath.c_str());
		}

		// Writes all content to file at once.
		static void write(const TCHAR* filePath, const BYTE* pData, size_t sz) {
			file fout;
			fout.open_or_create(filePath);
			fout.set_new_size(sz);
			fout.write(pData, sz);
		}

		// Writes all content to file at once.
		static void write(const wl::tstring& filePath, const BYTE* pData, size_t sz) {
			write(filePath.c_str(), pData, sz);
		}

		// Writes all content to file at once.
		static void write(const TCHAR* filePath, const std::vector<BYTE>& data) {
			write(filePath, &data[0], data.size());
		}

		// Writes all content to file at once.
		static void write(const wl::tstring& filePath, const std::vector<BYTE>& data) {
			write(filePath.c_str(), &data[0], data.size());
		}

		// Retrieves the file size in bytes.
		static size_t get_size(const TCHAR* filePath) {
			file ff;
			ff.open_existing(filePath, file::access::READONLY);
			return ff.size();
		}

		// Retrieves the file size in bytes.
		static size_t get_size(const wl::tstring& filePath) {
			return get_size(filePath.c_str());
		}

		// Gets creation, last access and last write dates.
		static dates get_dates(const TCHAR* filePath) {
			file ff;
			ff.open_existing(filePath, file::access::READONLY);
			return ff.get_dates();
		}

		// Gets creation, last access and last write dates.
		static dates get_dates(const wl::tstring& filePath) {
			return get_dates(filePath.c_str());
		}

		// Does the file exist on disk?
		static bool exists(const TCHAR* fileOrFolder) noexcept {
			return GetFileAttributes(fileOrFolder) != INVALID_FILE_ATTRIBUTES;
		}

		// Does the file exist on disk?
		static bool exists(const wl::tstring& fileOrFolder) noexcept {
			return exists(fileOrFolder.c_str());
		}

		// Is this path a directory?
		static bool is_dir(const TCHAR* thePath) noexcept {
			return (GetFileAttributes(thePath) & FILE_ATTRIBUTE_DIRECTORY) != 0;
		}

		// Is this path a directory?
		static bool is_dir(const wl::tstring& thePath) noexcept {
			return is_dir(thePath.c_str());
		}

		// Is the file hidden?
		static bool is_hidden(const TCHAR* thePath) noexcept {
			return (GetFileAttributes(thePath) & FILE_ATTRIBUTE_HIDDEN) != 0;
		}

		// Is the file hidden?
		static bool is_hidden(const wl::tstring& thePath) noexcept {
			return is_hidden(thePath.c_str());
		}

		// Deletes a file, or a directory recursively.
		static void del(const wl::tstring& fileOrFolder) {
			if (is_dir(fileOrFolder)) {
				// http://stackoverflow.com/q/1468774/6923555
				TCHAR szDir[MAX_PATH + 1]{}; // +1 for the double null terminate
				lstrcpy(szDir, fileOrFolder.c_str());

				SHFILEOPSTRUCT fos{};
				fos.wFunc  = FO_DELETE;
				fos.pFrom  = szDir;
				fos.fFlags = FOF_NO_UI;

				if (SHFileOperation(&fos)) {
					throw std::system_error(ERROR_INVALID_FUNCTION, std::system_category(), // arbitrary error code
						"SHFileOperation failed to recursively delete directory, can't specify error");
				}
			} else {
				if (!DeleteFile(fileOrFolder.c_str())) {
					throw std::system_error(GetLastError(), std::system_category(),
						"DeleteFile failed");
				}
			}
		}

		// Creates a new directory.
		static void create_dir(const TCHAR* thePath) {
			if (!CreateDirectory(thePath, nullptr)) {
				throw std::system_error(GetLastError(), std::system_category(),
					"CreateDirectory failed");
			}
		}

		// Creates a new directory.
		static void create_dir(const wl::tstring& thePath) {
			create_dir(thePath.c_str());
		}

		// List files within a directory according to a pattern, like "C:\\files\\*.txt". "*" will bring all.
		static std::vector<wl::tstring> list_dir(const wl::tstring& pathAndPattern) {
			std::vector<wl::tstring> files;

			WIN32_FIND_DATA wfd{};
			HANDLE hFind = FindFirstFile(pathAndPattern.c_str(), &wfd);
			if (hFind == INVALID_HANDLE_VALUE) {
				DWORD err = GetLastError();
				if (err == ERROR_FILE_NOT_FOUND) {
					return files;
				} else {
					throw std::system_error(err, std::system_category(),
						"FindFirstFile failed");
				}
			}

			wl::tstring pathPat = pathAndPattern.substr(0,
				pathAndPattern.find_last_of(_T('\\'))); // no trailing backslash
			do {
				if (*wfd.cFileName
					&& lstrcmpi(wfd.cFileName, _T(".")) // do not add current and parent paths
					&& lstrcmpi(wfd.cFileName, _T("..")))
				{
					files.emplace_back(pathPat);
					files.back().append(_T("\\")).append(wfd.cFileName);
				}
			} while (FindNextFile(hFind, &wfd));

			FindClose(hFind);
			return files;
		}

		// List files within a directory according to a pattern, like "C:\\files\\*.txt".
		static std::vector<wl::tstring> list_dir(const wl::tstring& dirPath, const wl::tstring& pattern) {
			wl::tstring pathAndPattern = dirPath;
			if (pathAndPattern.back() != _T('\\')) pathAndPattern.append(_T("\\"));
			pathAndPattern.append(pattern);
			return list_dir(pathAndPattern);
		}
	};
};

}//namespace wl
