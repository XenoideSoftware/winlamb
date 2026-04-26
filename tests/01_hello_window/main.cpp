// hello_window -- minimal winlamb sample: window_main + button + label.

#include <string>
#include <tchar.h>
#include <button.h>
#include <label.h>
#include <window_main.h>
#include <button.h>
#include <checkbox.h>
#include <com.h>
#include <combobox.h>
#include <datetime.h>
#include <datetime_picker.h>
#include <dialog_control.h>
#include <dialog_main.h>
#include <dialog_modal.h>
#include <dialog_modeless.h>
#include <download.h>
#include <executable.h>
#include <file.h>
#include <file_ini.h>
#include <file_mapped.h>
#include <font.h>
#include <gdi.h>
#include <icon.h>
#include <image_list.h>
#include <insert_order_map.h>
#include <label.h>
#include <listview.h>
#include <menu.h>
#include <path.h>
#include <progressbar.h>
#include <progress_taskbar.h>
#include <radio.h>
#include <radio_group.h>
#include <resizer.h>
#include <scrollinfo.h>
#include <statusbar.h>
#include <str.h>
#include <subclass.h>
#include <sysdlg.h>
#include <syspath.h>
#include <textbox.h>
#include <treeview.h>
#include <vec.h>
#include <version.h>
#include <window_control.h>
#include <window_main.h>
#include <wnd.h>
#include <xml.h>
#include <zip.h>

class HelloWindow final : public wl::window_main {
private:
    enum : int { ID_BTN_CLICK = 1001, ID_BTN_RESET = 1002 };

    wl::label  _info;
    wl::button _btnClick;
    wl::button _btnReset;
    int        _count = 0;

public:
    HelloWindow();
};

RUN(HelloWindow)

HelloWindow::HelloWindow() {
    setup.wndClassEx.lpszClassName = _T("WLAMB_HELLO_WINDOW");
    setup.title = _T("WinLamb \xB7 hello_window");
    setup.style |= WS_MINIMIZEBOX;
    setup.size = {420, 200};

    on_message(WM_CREATE, [this](wl::wm::create) -> LRESULT {
        _info.create(this, 0,
            _T("Click the button to update this label."),
            {16, 20}, {380, 22});
        _btnClick.create(this, ID_BTN_CLICK, _T("Click me"), {16, 60}, {120, 28});
        _btnReset.create(this, ID_BTN_RESET, _T("Reset"),    {150, 60}, {120, 28});
        return 0;
    });

    on_command(ID_BTN_CLICK, [this](wl::params) -> LRESULT {
        ++_count;
        _info.set_text(_T("You clicked the button ") +
            wl::to_tstring(_count) + _T(" time(s)."));
        return 0;
    });

    on_command(ID_BTN_RESET, [this](wl::params) -> LRESULT {
        _count = 0;
        _info.set_text(_T("Counter reset."));
        return 0;
    });
}
