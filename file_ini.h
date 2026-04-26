/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include "file_mapped.h"
#include "insert_order_map.h"
#include "str.h"
#include <tchar.h>
#include "internals/tstring.h"

namespace wl {

// Wrapper to INI file.
class file_ini final {
public:
	insert_order_map<wl::tstring, insert_order_map<wl::tstring, wl::tstring>> sections;

	const insert_order_map<wl::tstring, wl::tstring>& operator[](const wl::tstring& sectionName) const {
		return this->sections.operator[](sectionName);
	}

	insert_order_map<wl::tstring, wl::tstring>& operator[](const wl::tstring& sectionName) noexcept {
		return this->sections.operator[](sectionName);
	}

	file_ini& load_from_file(const TCHAR* filePath) {
		wl::tstring content = str::to_tstring(file_mapped::util::read(filePath));
		std::vector<wl::tstring> lines = str::split_lines(content);
		insert_order_map<wl::tstring, wl::tstring>* curSection = nullptr; // section-less keys will be ignored
		wl::tstring tmpName, tmpValue; // temporary buffers

		for (wl::tstring& line : lines) {
			if (str::trim(line).empty()) { // skip blank lines
				continue;
			} else if (line[0] == _T('[') && line.back() == _T(']')) { // begin of section found
				tmpName.clear();
				tmpName.insert(0, &line[1], line.length() - 2); // extract section name
				curSection = &this->sections[str::trim(tmpName)]; // if inexistent, will be inserted
			} else if (curSection && line[0] != _T(';') && line[0] != _T('#')) { // lines starting with ; or # will be ignored
				size_t idxEq = line.find_first_of(_T('='));
				if (idxEq != wl::tstring::npos) {
					tmpName.clear();
					tmpName.insert(0, &line[0], idxEq); // extract key name
					tmpValue.clear();
					tmpValue.insert(0, &line[idxEq + 1], line.length() - (idxEq + 1)); // extract value
					(*curSection)[tmpName] = tmpValue;
				}
			}
		}

		return *this;
	}

	void save_to_file(const TCHAR* filePath) const {
		file::util::write(filePath,
			str::to_utf8_blob(this->serialize(), str::write_bom::YES));
	}

	file_ini& load_from_file(const wl::tstring& filePath)     { return this->load_from_file(filePath.c_str()); }
	void      save_to_file(const wl::tstring& filePath) const { this->save_to_file(filePath.c_str()); }

	// Returns the INI contents as a string, ready to be written to a file.
	wl::tstring serialize() const {
		wl::tstring out;
		bool isFirst = true;

		using sectionT = insert_order_map<wl::tstring, insert_order_map<wl::tstring, wl::tstring>>::entry;
		using entryT = insert_order_map<wl::tstring, wl::tstring>::entry;

		for (const sectionT& sectionEntry : this->sections) {
			if (isFirst) {
				isFirst = false;
			} else {
				out.append(_T("\r\n"));
			}
			out.append(_T("[")).append(sectionEntry.key).append(_T("]\r\n"));

			for (const entryT& keyEntry : sectionEntry.value) {
				out.append(keyEntry.key).append(_T("="))
					.append(keyEntry.value).append(_T("\r\n"));
			}
		}
		return out;
	}

	// Checks INI file structure against "[section1]keyA,keyB,keyC[section2]keyX,keyY".
	bool structure_is(const wl::tstring& structure) const {
		using strvecT = std::vector<wl::tstring>;
		for (const insert_order_map<wl::tstring, strvecT>::entry& descrSectionEntry : this->_parse_structure(structure)) {
			const insert_order_map<wl::tstring, wl::tstring>* pCurSection = this->sections.get_if_exists(descrSectionEntry.key);
			if (!pCurSection) return false; // section name not found
			for (const wl::tstring& descrKeyEntry : descrSectionEntry.value) {
				if (!pCurSection->has(descrKeyEntry)) return false; // key name not found
			}
		}
		return true;
	}

private:
	insert_order_map<wl::tstring, std::vector<wl::tstring>> _parse_structure(const wl::tstring& structure) const {
		using strvecT = std::vector<wl::tstring>;
		insert_order_map<wl::tstring, strvecT> parsed;
		strvecT secBlocks = str::split(structure, _T("["));
		for (wl::tstring& secBlock : secBlocks) {
			if (secBlock.empty()) continue;
			size_t endSecIdx = secBlock.find_first_of(_T(']'));
			strvecT& curSec = parsed[secBlock.substr(0, endSecIdx)];
			secBlock.erase(0, endSecIdx + 1);
			curSec = str::split(secBlock, _T(","));
		}
		return parsed;
	}
};

}//namespace wl
