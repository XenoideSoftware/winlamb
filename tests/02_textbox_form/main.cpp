// textbox_form -- exercises textbox, checkbox, radio + radio_group, sysdlg::msgbox.

#include <string>
#include <tchar.h>

#include <button.h>
#include <checkbox.h>
#include <label.h>
#include <radio.h>
#include <radio_group.h>
#include <sysdlg.h>
#include <textbox.h>
#include <window_main.h>

class FormWindow final : public wl::window_main {
private:
    enum : int {
        ID_TXT_NAME = 100,
        ID_TXT_AGE,
        ID_TXT_NOTES,
        ID_RAD_M,
        ID_RAD_F,
        ID_RAD_OTHER,
        ID_CHK_TERMS,
        ID_BTN_OK,
    };

    wl::label       _lblName, _lblAge, _lblGender, _lblNotes;
    wl::textbox     _txtName, _txtAge, _txtNotes;
    wl::radio       _radM, _radF, _radOther;
    wl::radio_group _gender;
    wl::checkbox    _chkTerms;
    wl::button      _btnOk;

public:
    FormWindow();
};

RUN(FormWindow)

FormWindow::FormWindow() {
    setup.wndClassEx.lpszClassName = _T("WLAMB_FORM");
    setup.title = _T("WinLamb \xB7 textbox_form");
    setup.style |= WS_MINIMIZEBOX;
    setup.size = {460, 360};

    on_message(WM_CREATE, [this](wl::wm::create) -> LRESULT {
        _lblName.create(this, 0, _T("Name:"), {16, 18}, {70, 20});
        _txtName.create(this, ID_TXT_NAME, wl::textbox::type::NORMAL, {96, 16}, 320);

        _lblAge.create(this, 0, _T("Age:"), {16, 50}, {70, 20});
        _txtAge.create(this, ID_TXT_AGE, wl::textbox::type::NORMAL, {96, 48}, 90);

        _lblGender.create(this, 0, _T("Gender:"), {16, 84}, {70, 20});
        _radM    .create(this, ID_RAD_M,     _T("Male"),   { 96, 82}, {70, 20});
        _radF    .create(this, ID_RAD_F,     _T("Female"), {170, 82}, {80, 20});
        _radOther.create(this, ID_RAD_OTHER, _T("Other"),  {254, 82}, {80, 20});
        _gender.assign(this, {ID_RAD_M, ID_RAD_F, ID_RAD_OTHER});

        _lblNotes.create(this, 0, _T("Notes:"), {16, 118}, {70, 20});
        _txtNotes.create(this, ID_TXT_NOTES, wl::textbox::type::MULTILINE,
            {96, 116}, 320, 120);

        _chkTerms.create(this, ID_CHK_TERMS, _T("I accept the terms"),
            {16, 248}, {300, 22});

        _btnOk.create(this, ID_BTN_OK, _T("Submit"), {16, 282}, {120, 30});
        return 0;
    });

    on_command(ID_BTN_OK, [this](wl::params) -> LRESULT {
        if (!_chkTerms.is_checked()) {
            wl::sysdlg::msgbox(hwnd(), _T("Form"),
                _T("Please accept the terms before submitting."),
                MB_ICONWARNING);
            return 0;
        }

        const wl::radio& chosen = _gender.get_by_id(_gender.get_checked_id());
        wl::tstring summary =
            _T("Name:   ") + _txtName.get_text() + _T("\r\n")
            _T("Age:    ") + _txtAge.get_text()  + _T("\r\n")
            _T("Gender: ") + chosen.get_text()   + _T("\r\n")
            _T("Notes:\r\n")  + _txtNotes.get_text();

        wl::sysdlg::msgbox(hwnd(), _T("Submitted"), summary, MB_ICONINFORMATION);
        return 0;
    });
}
