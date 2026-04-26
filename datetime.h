/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <Windows.h>
#include <tchar.h>

namespace wl {

// Wrapper to SYSTEMTIME structure.
class datetime final {
private:
	SYSTEMTIME _st{};

public:
	datetime() noexcept                     { this->set_now(); }
	datetime(LONGLONG ms) noexcept          { this->operator=(ms); }
	datetime(const SYSTEMTIME& st) noexcept { this->operator=(st); }
	datetime(const FILETIME& ft) noexcept   { this->operator=(ft); }

	const SYSTEMTIME& systemtime() const noexcept {
		return this->_st;
	}

	datetime& operator=(LONGLONG ms) noexcept {
		SecureZeroMemory(&this->_st, sizeof(SYSTEMTIME));

		this->_st.wMilliseconds = ms % 1000;
		ms = (ms - this->_st.wMilliseconds) / 1000; // now in seconds
		this->_st.wSecond = ms % 60;
		ms = (ms - this->_st.wSecond) / 60; // now in minutes
		this->_st.wMinute = ms % 60;
		ms = (ms - this->_st.wMinute) / 60; // now in hours
		this->_st.wHour = ms % 24;
		ms = (ms - this->_st.wHour) / 24; // now in days
		return *this;
	}

	datetime& operator=(const SYSTEMTIME& st) noexcept {
		memcpy(&this->_st, &st, sizeof(SYSTEMTIME));
		return *this;
	}

	datetime& operator=(const FILETIME& ft) noexcept {
		SYSTEMTIME st1{};
		FileTimeToSystemTime(&ft, &st1);

		TIME_ZONE_INFORMATION tzi{};
		GetTimeZoneInformation(&tzi);
		SystemTimeToTzSpecificLocalTime(&tzi, &st1, &this->_st);
		return *this;
	}

	datetime& set_now() noexcept {
		SYSTEMTIME st1{};
		GetSystemTime(&st1);

		TIME_ZONE_INFORMATION tzi{};
		GetTimeZoneInformation(&tzi);
		SystemTimeToTzSpecificLocalTime(&tzi, &st1, &this->_st);
		return *this;
	}

	LONGLONG timestamp() const noexcept {
		// http://www.frenk.com/2009/12/convert-filetime-to-unix-timestamp/
		LARGE_INTEGER date, adjust;
		_st_to_li(this->_st, date);
		adjust.QuadPart = 11644473600000 * 10000; // 100-nanoseconds = milliseconds * 10000
		date.QuadPart -= adjust.QuadPart; // removes the diff between 1970 and 1601
		//return date.QuadPart / 10000000; // converts back from 100-nanoseconds to seconds
		return date.QuadPart / 10000; // to milliseconds; to printf use %I64u
	}

	size_t ms_diff_from(const datetime& other) const noexcept {
		LARGE_INTEGER liUs, liThem;
		_st_to_li(this->_st, liUs);
		_st_to_li(other._st, liThem);

		// 100-nanoseconds to milliseconds; to printf a LARGE_INTEGER use %I64u.
		// To int32, max is 1,193 hours; to int64, a shitload of hours.
		return static_cast<size_t>((liUs.QuadPart - liThem.QuadPart) / 10000);
	}

	datetime& add_ms(LONGLONG ms) noexcept {
		LARGE_INTEGER li;
		_st_to_li(this->_st, li);
		li.QuadPart += ms * 10000; // milliseconds to 100-nanoseconds
		_li_to_st(li, this->_st);
		return *this;
	}

	datetime& add_sec(LONGLONG sec) noexcept { return this->add_ms(sec * 1000); }
	datetime& add_min(LONGLONG min) noexcept { return this->add_sec(min * 60); }
	datetime& add_hour(LONGLONG h) noexcept  { return this->add_min(h * 60); }
	datetime& add_day(LONGLONG d) noexcept   { return this->add_hour(d * 24); }

	const TCHAR* name_month() const noexcept {
		const TCHAR* months[] = { _T("January"), _T("February"), _T("March"), _T("April"), _T("May"), _T("June"),
			_T("July"), _T("August"), _T("September"), _T("October"), _T("November"), _T("December") };
		return months[this->_st.wMonth - 1];
	}

	const TCHAR* name_month_short() const noexcept {
		const TCHAR* months[] = { _T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"), _T("May"), _T("Jun"),
			_T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec") };
		return months[this->_st.wMonth - 1];
	}

	const TCHAR* name_weekday() const noexcept {
		const TCHAR* weekday[] = { _T("Sunday"), _T("Monday"), _T("Tuesday"), _T("Wednesday"),
			_T("Thursday"), _T("Friday"), _T("Saturday") };
		return weekday[this->_st.wDayOfWeek];
	}

	const TCHAR* name_weekday_short() const noexcept {
		const TCHAR* weekday[] = { _T("Sun"), _T("Mon"), _T("Tue"), _T("Wed"), _T("Thu"), _T("Fri"), _T("Sat") };
		return weekday[this->_st.wDayOfWeek];
	}

private:
	static void _st_to_li(const SYSTEMTIME& st, LARGE_INTEGER& li) noexcept {
		FILETIME ft{};
		SystemTimeToFileTime(&st, &ft);

		li.HighPart = ft.dwHighDateTime;
		li.LowPart = ft.dwLowDateTime;
	}

	static void _li_to_st(const LARGE_INTEGER& li, SYSTEMTIME& st) noexcept {
		FILETIME ft{};
		ft.dwHighDateTime = li.HighPart;
		ft.dwLowDateTime = li.LowPart;

		FileTimeToSystemTime(&ft, &st);
	}
};

}//namespace wl
