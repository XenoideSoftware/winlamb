// treeview_demo -- exercises treeview with nested items and a WM_NOTIFY
// handler that reacts to TVN_SELCHANGED.

#include <string>
#include <tchar.h>

#include <label.h>
#include <treeview.h>
#include <window_main.h>

class TreeViewDemo final : public wl::window_main {
private:
    enum : int { ID_TV = 100 };

    wl::treeview _tv;
    wl::label    _lblSel;

    static wl::tstring item_text(HWND hTree, HTREEITEM hItem) {
        TCHAR buf[256] = {0};
        TVITEM tvi{};
        tvi.mask = TVIF_TEXT | TVIF_HANDLE;
        tvi.hItem = hItem;
        tvi.pszText = buf;
        tvi.cchTextMax = ARRAYSIZE(buf);
        TreeView_GetItem(hTree, &tvi);
        return buf;
    }

    static wl::tstring item_path(HWND hTree, HTREEITEM hItem) {
        wl::tstring path;
        while (hItem) {
            wl::tstring t = item_text(hTree, hItem);
            path = path.empty() ? t : t + _T(" / ") + path;
            hItem = TreeView_GetParent(hTree, hItem);
        }
        return path;
    }

public:
    TreeViewDemo();
};

RUN(TreeViewDemo)

TreeViewDemo::TreeViewDemo() {
    setup.wndClassEx.lpszClassName = _T("WLAMB_TREEVIEW_DEMO");
    setup.title = _T("WinLamb \xB7 treeview_demo");
    setup.style |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
    setup.size = {520, 420};

    on_message(WM_CREATE, [this](wl::wm::create) -> LRESULT {
        _tv.create(this, ID_TV, {16, 16}, {480, 320});
        _tv.style.set_style(true,
            TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS);

        auto src = _tv.items.add_root(_T("src"));
        auto core = src.add_child(_T("xenoide-core"));
        core.add_child(_T("FileService.h"));
        core.add_child(_T("WorkspaceService.h"));
        auto ui = src.add_child(_T("xenoide-ui-folderexplorer"));
        ui.add_child(_T("FolderExplorer.h"));
        ui.add_child(_T("FolderExplorerPresenter.cpp"));

        auto cfg = _tv.items.add_root(_T("config"));
        cfg.add_child(_T("cpp.json"));
        cfg.add_child(_T("glsl.json"));
        cfg.add_child(_T("cmake.json"));

        TreeView_Expand(_tv.hwnd(), src.htreeitem(),  TVE_EXPAND);
        TreeView_Expand(_tv.hwnd(), core.htreeitem(), TVE_EXPAND);

        _lblSel.create(this, 0, _T("Selected: (none)"), {16, 348}, {480, 22});
        return 0;
    });

    on_message(WM_SIZE, [this](wl::wm::size p) -> LRESULT {
        if (p.is_minimized()) return 0;
        const SIZE sz = p.sz();
        SetWindowPos(_tv.hwnd(), nullptr,
            16, 16, sz.cx - 32, sz.cy - 70,
            SWP_NOZORDER | SWP_NOACTIVATE);
        SetWindowPos(_lblSel.hwnd(), nullptr,
            16, sz.cy - 40, sz.cx - 32, 22,
            SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    });

    on_notify(ID_TV, TVN_SELCHANGED, [this](wl::params p) -> LRESULT {
        auto* pnmtv = reinterpret_cast<NMTREEVIEW*>(p.lParam);
        _lblSel.set_text(_T("Selected: ") + item_path(_tv.hwnd(), pnmtv->itemNew.hItem));
        return 0;
    });
}
