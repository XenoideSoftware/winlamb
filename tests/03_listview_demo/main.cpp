// listview_demo -- exercises listview in REPORT mode with columns,
// add/remove rows, and parent-side WM_SIZE handling.

#include <string>
#include <tchar.h>

#include <button.h>
#include <label.h>
#include <listview.h>
#include <textbox.h>
#include <window_main.h>

class ListViewDemo final : public wl::window_main {
private:
    enum : int {
        ID_LV = 100,
        ID_TXT_NAME,
        ID_TXT_CITY,
        ID_BTN_ADD,
        ID_BTN_REMOVE,
        ID_BTN_CLEAR,
    };

    wl::label    _lblName, _lblCity, _lblHint;
    wl::textbox  _txtName, _txtCity;
    wl::button   _btnAdd, _btnRemove, _btnClear;
    wl::listview _lv;
    int          _nextId = 1;

public:
    ListViewDemo();
};

RUN(ListViewDemo)

ListViewDemo::ListViewDemo() {
    setup.wndClassEx.lpszClassName = _T("WLAMB_LISTVIEW_DEMO");
    setup.title = _T("WinLamb \xB7 listview_demo");
    setup.style |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
    setup.size = {620, 420};

    on_message(WM_CREATE, [this](wl::wm::create) -> LRESULT {
        _lblName.create(this, 0, _T("Name:"), {16, 16}, {50, 20});
        _txtName.create(this, ID_TXT_NAME, wl::textbox::type::NORMAL, {68, 14}, 160);

        _lblCity.create(this, 0, _T("City:"), {238, 16}, {40, 20});
        _txtCity.create(this, ID_TXT_CITY, wl::textbox::type::NORMAL, {280, 14}, 140);

        _btnAdd   .create(this, ID_BTN_ADD,    _T("Add"),            {430, 12}, {70, 24});
        _btnRemove.create(this, ID_BTN_REMOVE, _T("Remove sel."),    {506, 12}, {90, 24});

        _lv.create(this, ID_LV, {16, 48}, {580, 300},
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS,
            WS_EX_CLIENTEDGE);
        _lv.style.full_row_select(true);
        _lv.style.grid_lines(true);
        _lv.style.double_buffer(true);
        _lv.columns
            .add(_T("#"),    40)
            .add(_T("Name"), 220)
            .add(_T("City"), 220);

        // seed rows
        _lv.items.add(_T("1")).set_text(_T("Ada Lovelace"),   1).set_text(_T("London"),    2);
        _lv.items.add(_T("2")).set_text(_T("Grace Hopper"),   1).set_text(_T("New York"),  2);
        _lv.items.add(_T("3")).set_text(_T("Linus Torvalds"), 1).set_text(_T("Helsinki"),  2);
        _nextId = 4;

        _btnClear.create(this, ID_BTN_CLEAR, _T("Clear all"), {16, 358}, {110, 26});
        _lblHint.create(this, 0,
            _T("Tip: Ctrl+A selects all rows in the listview."),
            {136, 362}, {460, 20});
        return 0;
    });

    on_message(WM_SIZE, [this](wl::wm::size p) -> LRESULT {
        if (p.is_minimized()) return 0;
        const SIZE sz = p.sz();
        SetWindowPos(_lv.hwnd(), nullptr,
            16, 48, sz.cx - 32, sz.cy - 100,
            SWP_NOZORDER | SWP_NOACTIVATE);
        SetWindowPos(_btnClear.hwnd(), nullptr,
            16, sz.cy - 42, 110, 26,
            SWP_NOZORDER | SWP_NOACTIVATE);
        SetWindowPos(_lblHint.hwnd(), nullptr,
            136, sz.cy - 38, sz.cx - 152, 20,
            SWP_NOZORDER | SWP_NOACTIVATE);
        _lv.columns.set_width_to_fill(2);
        return 0;
    });

    on_command(ID_BTN_ADD, [this](wl::params) -> LRESULT {
        wl::tstring name = _txtName.get_text();
        wl::tstring city = _txtCity.get_text();
        if (name.empty()) {
            name = _T("(unnamed)");
        }
        _lv.items
            .add(wl::to_tstring(_nextId++))
            .set_text(name, 1)
            .set_text(city, 2);
        _txtName.set_text(_T(""));
        _txtCity.set_text(_T(""));
        return 0;
    });

    on_command(ID_BTN_REMOVE, [this](wl::params) -> LRESULT {
        // walk selected items in reverse so deletions don't shift indices
        int i = -1;
        std::vector<int> selected;
        while ((i = ListView_GetNextItem(_lv.hwnd(), i, LVNI_SELECTED)) != -1) {
            selected.push_back(i);
        }
        for (auto it = selected.rbegin(); it != selected.rend(); ++it) {
            _lv.items[static_cast<size_t>(*it)].remove();
        }
        return 0;
    });

    on_command(ID_BTN_CLEAR, [this](wl::params) -> LRESULT {
        ListView_DeleteAllItems(_lv.hwnd());
        _nextId = 1;
        return 0;
    });
}
