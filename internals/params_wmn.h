/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include "params_wm.h"
#include <CommCtrl.h>

namespace wl {

// Message crackers for all documented WM_NOTIFY messages.
namespace wmn {

#define WINLAMB_NOTIFYWM(sname, oname) \
	struct sname : public wm::notify { \
		sname(const params& p) noexcept : notify(p) { } \
		oname& nmhdr() const noexcept { return *reinterpret_cast<oname*>(this->lParam); } \
	};

	// ComboBoxEx notifications.
	namespace cben {
		WINLAMB_NOTIFYWM(beginedit, NMHDR);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(deleteitem, NMCOMBOBOXEX);
		WINLAMB_NOTIFYWM(dragbegin, NMCBEDRAGBEGIN);
		WINLAMB_NOTIFYWM(endedit, NMCBEENDEDIT);
		WINLAMB_NOTIFYWM(getdispinfo, NMCOMBOBOXEX);
		WINLAMB_NOTIFYWM(insertitem, NMCOMBOBOXEX);
		WINLAMB_NOTIFYWM(setcursor, NMMOUSE);
#endif
	}

	// Date and time picker notifications.
	namespace dtn {
		WINLAMB_NOTIFYWM(closeup, NMHDR);
		WINLAMB_NOTIFYWM(datetimechange, NMDATETIMECHANGE);
		WINLAMB_NOTIFYWM(dropdown, NMHDR);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(format, NMDATETIMEFORMAT);
#endif
#if _WIN32_WINNT >= 0x0600
		WINLAMB_NOTIFYWM(formatquery, NMDATETIMEFORMATQUERY);
#endif
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(userstring, NMDATETIMESTRING);
		WINLAMB_NOTIFYWM(wmkeydown, NMDATETIMEWMKEYDOWN);
#endif
		WINLAMB_NOTIFYWM(killfocus, NMHDR);
		WINLAMB_NOTIFYWM(setfocus, NMHDR);
	}

	// List view notifications.
	namespace lvn {
		WINLAMB_NOTIFYWM(begindrag, NMLISTVIEW);
		WINLAMB_NOTIFYWM(beginlabeledit, NMLVDISPINFO);
		WINLAMB_NOTIFYWM(beginrdrag, NMLISTVIEW);
#if _WIN32_WINNT >= 0x0600
		WINLAMB_NOTIFYWM(beginscroll, NMLVSCROLL);
#endif
		WINLAMB_NOTIFYWM(columnclick, NMLISTVIEW);
		WINLAMB_NOTIFYWM(columndropdown, NMLISTVIEW);
		WINLAMB_NOTIFYWM(columnoverflowclick, NMLISTVIEW);
		WINLAMB_NOTIFYWM(deleteallitems, NMLISTVIEW);
		WINLAMB_NOTIFYWM(deleteitem, NMLISTVIEW);
		WINLAMB_NOTIFYWM(endlabeledit, NMLVDISPINFO);
#if _WIN32_WINNT >= 0x0600
		WINLAMB_NOTIFYWM(endscroll, NMLVSCROLL);
#endif
		WINLAMB_NOTIFYWM(getdispinfo, NMLVDISPINFO);
#if _WIN32_WINNT >= 0x0600
		WINLAMB_NOTIFYWM(getemptymarkup, NMLVEMPTYMARKUP);
#endif
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(getinfotip, NMLVGETINFOTIP);
#endif
		WINLAMB_NOTIFYWM(hottrack, NMLISTVIEW);
		WINLAMB_NOTIFYWM(incrementalsearch, NMLVFINDITEM);
		WINLAMB_NOTIFYWM(insertitem, NMLISTVIEW);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(itemactivate, NMITEMACTIVATE);
#endif
		WINLAMB_NOTIFYWM(itemchanged, NMLISTVIEW);
		WINLAMB_NOTIFYWM(itemchanging, NMLISTVIEW);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(keydown, NMLVKEYDOWN);
#endif
#if _WIN32_WINNT >= 0x0600
		WINLAMB_NOTIFYWM(linkclick, NMLVLINK);
#endif
		WINLAMB_NOTIFYWM(marqueebegin, NMHDR);
		WINLAMB_NOTIFYWM(odcachehint, NMLVCACHEHINT);
		WINLAMB_NOTIFYWM(odfinditem, NMLVFINDITEM);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(odstatechanged, NMLVODSTATECHANGE);
#endif
		WINLAMB_NOTIFYWM(setdispinfo, NMLVDISPINFO);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(click, NMITEMACTIVATE);
#endif
		WINLAMB_NOTIFYWM(customdraw, NMLVCUSTOMDRAW);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(dblclk, NMITEMACTIVATE);
#endif
		WINLAMB_NOTIFYWM(hover, NMHDR);
		WINLAMB_NOTIFYWM(killfocus, NMHDR);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(rclick, NMITEMACTIVATE);
		WINLAMB_NOTIFYWM(rdblclk, NMITEMACTIVATE);
#endif
		WINLAMB_NOTIFYWM(releasedcapture, NMHDR);
		WINLAMB_NOTIFYWM(return_, NMHDR);
		WINLAMB_NOTIFYWM(setfocus, NMHDR);
	}

	// Month calendar notifications.
	namespace mcn {
		WINLAMB_NOTIFYWM(getdaystate, NMDAYSTATE);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(selchange, NMSELCHANGE);
		WINLAMB_NOTIFYWM(select, NMSELCHANGE);
#endif
#if _WIN32_WINNT >= 0x0600
		WINLAMB_NOTIFYWM(viewchange, NMVIEWCHANGE);
#endif
		WINLAMB_NOTIFYWM(releasedcapture, NMHDR);
	}

	// Status bar notifications.
	namespace sbn {
		WINLAMB_NOTIFYWM(simplemodechange, NMHDR);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(click, NMMOUSE);
		WINLAMB_NOTIFYWM(dblclk, NMMOUSE);
		WINLAMB_NOTIFYWM(rclick, NMMOUSE);
		WINLAMB_NOTIFYWM(rdblclk, NMMOUSE);
#endif
	}

	// Tab control notifications.
	namespace tcn {
		WINLAMB_NOTIFYWM(focuschange, NMHDR);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(getobject, NMOBJECTNOTIFY);
#endif
		WINLAMB_NOTIFYWM(keydown, NMTCKEYDOWN);
		WINLAMB_NOTIFYWM(selchange, NMHDR);
		WINLAMB_NOTIFYWM(selchanging, NMHDR);
		WINLAMB_NOTIFYWM(click, NMHDR);
		WINLAMB_NOTIFYWM(dblclk, NMHDR);
		WINLAMB_NOTIFYWM(rclick, NMHDR);
		WINLAMB_NOTIFYWM(rdblclk, NMHDR);
		WINLAMB_NOTIFYWM(releasedcapture, NMHDR);
	}

	// Trackbar notifications.
	namespace trbn {
#if _WIN32_WINNT >= 0x0600
		WINLAMB_NOTIFYWM(thumbposchanging, NMTRBTHUMBPOSCHANGING);
#endif
		WINLAMB_NOTIFYWM(customdraw, NMCUSTOMDRAW);
		WINLAMB_NOTIFYWM(releasedcapture, NMHDR);
	}

	// Tooltip notifications.
	namespace ttn {
		WINLAMB_NOTIFYWM(getdispinfo, NMTTDISPINFO);
		WINLAMB_NOTIFYWM(linkclick, NMHDR);
		WINLAMB_NOTIFYWM(needtext, NMTTDISPINFO);
		WINLAMB_NOTIFYWM(pop, NMHDR);
		WINLAMB_NOTIFYWM(show, NMHDR);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(customdraw, NMTTCUSTOMDRAW);
#endif
	}

	// Tree view notifications.
	namespace tvn {
#if _WIN32_WINNT >= 0x0600
		WINLAMB_NOTIFYWM(asyncdraw, NMTVASYNCDRAW);
#endif
		WINLAMB_NOTIFYWM(begindrag, NMTREEVIEW);
		WINLAMB_NOTIFYWM(beginlabeledit, NMTVDISPINFO);
		WINLAMB_NOTIFYWM(beginrdrag, NMTREEVIEW);
		WINLAMB_NOTIFYWM(deleteitem, NMTREEVIEW);
		WINLAMB_NOTIFYWM(endlabeledit, NMTVDISPINFO);
		WINLAMB_NOTIFYWM(getdispinfo, NMTVDISPINFO);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(getinfotip, NMTVGETINFOTIP);
#endif
#if _WIN32_WINNT >= 0x0600
		WINLAMB_NOTIFYWM(itemchanged, NMTVITEMCHANGE);
		WINLAMB_NOTIFYWM(itemchanging, NMTVITEMCHANGE);
#endif
		WINLAMB_NOTIFYWM(itemexpanded, NMTREEVIEW);
		WINLAMB_NOTIFYWM(itemexpanding, NMTREEVIEW);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(keydown, NMTVKEYDOWN);
#endif
		WINLAMB_NOTIFYWM(selchanged, NMTREEVIEW);
		WINLAMB_NOTIFYWM(selchanging, NMTREEVIEW);
		WINLAMB_NOTIFYWM(setdispinfo, NMTVDISPINFO);
		WINLAMB_NOTIFYWM(singleexpand, NMTREEVIEW);
		WINLAMB_NOTIFYWM(click, NMHDR);
		WINLAMB_NOTIFYWM(customdraw, NMTVCUSTOMDRAW);
		WINLAMB_NOTIFYWM(dblclk, NMHDR);
		WINLAMB_NOTIFYWM(killfocus, NMHDR);
		WINLAMB_NOTIFYWM(rclick, NMHDR);
		WINLAMB_NOTIFYWM(rdblclk, NMHDR);
		WINLAMB_NOTIFYWM(return_, NMHDR);
#if _WIN32_WINNT >= 0x0501
		WINLAMB_NOTIFYWM(setcursor, NMMOUSE);
#endif
		WINLAMB_NOTIFYWM(setfocus, NMHDR);
	}

	// Up-down notifications.
	namespace udn {
		WINLAMB_NOTIFYWM(deltapos, NMUPDOWN);
		WINLAMB_NOTIFYWM(releasedcapture, NMHDR);
	}

}//namespace wmn
}//namespace wl
