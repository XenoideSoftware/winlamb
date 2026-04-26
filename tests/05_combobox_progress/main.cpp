// combobox_progress -- exercises combobox, progressbar and statusbar.
//
// A combobox picks a "speed". A Start/Stop button drives the progressbar
// either as a determinate fill (using SetTimer) or as a marquee. The
// statusbar reflects the current state across two parts.

#include <string>
#include <tchar.h>

#include <button.h>
#include <combobox.h>
#include <label.h>
#include <progressbar.h>
#include <statusbar.h>
#include <window_main.h>

class ComboProgressDemo final : public wl::window_main {
private:
    enum : int {
        ID_CB_SPEED  = 100,
        ID_BTN_START,
        ID_BTN_STOP,
        ID_BTN_MARQUEE,
        ID_TIMER_TICK = 1,
    };

    wl::label       _lblSpeed;
    wl::combobox    _cbSpeed;
    wl::button      _btnStart, _btnStop, _btnMarquee;
    wl::progressbar _pb;
    wl::statusbar   _sb;
    bool            _running = false;
    bool            _marquee = false;

    int speed_step() const noexcept {
        switch (_cbSpeed.get_selected_index()) {
            case 0:  return 1;   // slow
            case 1:  return 3;   // normal
            case 2:  return 7;   // fast
            default: return 3;
        }
    }

    void update_status(const TCHAR* state) {
        _sb.set_text(state, 0);
        _sb.set_text(_T("Speed: ") + _cbSpeed.get_selected_text(), 1);
    }

public:
    ComboProgressDemo();
};

RUN(ComboProgressDemo)

ComboProgressDemo::ComboProgressDemo() {
    setup.wndClassEx.lpszClassName = _T("WLAMB_COMBO_PROGRESS");
    setup.title = _T("WinLamb \xB7 combobox_progress");
    setup.style |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
    setup.size = {520, 220};

    on_message(WM_CREATE, [this](wl::wm::create) -> LRESULT {
        _lblSpeed.create(this, 0, _T("Speed:"), {16, 18}, {50, 20});
        _cbSpeed.create(this, ID_CB_SPEED, {68, 14}, 140, wl::combobox::sort::UNSORTED);
        _cbSpeed.add({_T("Slow"), _T("Normal"), _T("Fast")});
        _cbSpeed.select(1);

        _btnStart  .create(this, ID_BTN_START,   _T("Start"),   {220, 12}, {80, 26});
        _btnStop   .create(this, ID_BTN_STOP,    _T("Stop"),    {306, 12}, {80, 26});
        _btnMarquee.create(this, ID_BTN_MARQUEE, _T("Marquee"), {392, 12}, {90, 26});

        _pb.create(this, 0, nullptr, {16, 60}, {466, 26});
        _pb.set_range(0, 100).set_pos(0);

        _sb.create(this);
        _sb.add_resizable_part(2)  // state
           .add_resizable_part(1); // speed
        update_status(_T("Idle"));
        return 0;
    });

    on_message(WM_SIZE, [this](wl::wm::size p) -> LRESULT {
        if (p.is_minimized()) return 0;
        const SIZE sz = p.sz();
        SetWindowPos(_pb.hwnd(), nullptr,
            16, 60, sz.cx - 32, 26, SWP_NOZORDER | SWP_NOACTIVATE);
        _sb.adjust(p);
        return 0;
    });

    on_command(ID_BTN_START, [this](wl::params) -> LRESULT {
        if (_marquee) {
            _pb.set_waiting(false);
            _marquee = false;
        }
        _pb.set_pos(0);
        SetTimer(hwnd(), ID_TIMER_TICK, 80, nullptr);
        _running = true;
        update_status(_T("Running"));
        return 0;
    });

    on_command(ID_BTN_STOP, [this](wl::params) -> LRESULT {
        KillTimer(hwnd(), ID_TIMER_TICK);
        _running = false;
        if (_marquee) {
            _pb.set_waiting(false);
            _marquee = false;
        }
        update_status(_T("Stopped"));
        return 0;
    });

    on_command(ID_BTN_MARQUEE, [this](wl::params) -> LRESULT {
        if (_running) {
            KillTimer(hwnd(), ID_TIMER_TICK);
            _running = false;
        }
        _marquee = !_marquee;
        _pb.set_waiting(_marquee);
        update_status(_marquee ? _T("Marquee") : _T("Idle"));
        return 0;
    });

    // CBN_SELCHANGE is sent through WM_COMMAND; on_command catches the id.
    on_command(ID_CB_SPEED, [this](wl::params) -> LRESULT {
        update_status(_running ? _T("Running") : (_marquee ? _T("Marquee") : _T("Idle")));
        return 0;
    });

    on_message(WM_TIMER, [this](wl::params p) -> LRESULT {
        if (p.wParam != ID_TIMER_TICK) return 0;
        int next = _pb.get_pos() + speed_step();
        if (next >= 100) {
            _pb.set_pos(100);
            KillTimer(hwnd(), ID_TIMER_TICK);
            _running = false;
            update_status(_T("Done"));
        } else {
            _pb.set_pos(next);
        }
        return 0;
    });
}
