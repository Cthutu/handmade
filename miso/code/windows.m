-- Windows header file

export
{
    -- Handles
    HANDLE: ^void
    HINSTANCE: HANDLE
    HWND: HANDLE

    -- Integers
    UINT: u32

    -- Strings
    LPSTR: ^CHAR
    CHAR: char
    LPWSTR: ^WCHAR
    WCHAR: ^rune

    -- Unicode version USER32 functions
    foreign(user32, c) MessageBoxW: (hWnd HWND, lpText LPWSTR, lpCaption LPWSTR, uType UINT) -> int

    -- ASCII version USER32 functions
    foreign(user32, c) MessageBoxA: (hWnd HWND, lpText LPSTR, lpCaption LPSTR, uType UINT) -> int

    -- Default versions
    MessageBox:                 MessageBoxA

    -- Constants
    MB_ABORTRETRYIGNORE:        0x00000002
    MB_CANCELTRYCONTINUE:       0x00000006
    MB_HELP:                    0x00004000
    MB_OK:                      0x00000000
    MB_OKCANCEL:                0x00000001
    MB_RETRYCANCEL:             0x00000005
    MB_YESNO:                   0x00000004
    MB_YESNOCANCEL:             0x00000003

    MB_ICONEXCLAMATION:         0x00000030
    MB_ICONWARNING:             0x00000030
    MB_ICONINFORMATION:         0x00000040
    MB_ICONASTERISK:            0x00000040
    MB_ICONQUESTION:            0x00000020
    MB_ICONSTOP:                0x00000010
    MB_ICONERROR:               0x00000010
    MB_ICONHAND:                0x00000010

    MB_DEFBUTTON1:              0x00000000
    MB_DEFBUTTON2:              0x00000100
    MB_DEFBUTTON3:              0x00000200
    MB_DEFBUTTON4:              0x00000300

    MB_APPLMODAL:               0x00000000
    MB_SYSTEMMODAL:             0x00001000
    MB_TASKMODEL:               0x00002000

    MB_DEFAULT_DESKTOP_ONLY:    0x00020000
    MB_RIGHT:                   0x00080000
    MB_RTLREADING:              0x00100000
    MB_SETFOREGROUND:           0x00010000
    MB_TOPMOST:                 0x00040000
    MB_SERVICE_NOTIFICATION:    0x00200000

    IDOK:                       1
    IDCANCEL:                   2
    IDABORT:                    3
    IDRETRY:                    4
    IDIGNORE:                   5
    IDYES:                      6
    IDNO:                       7
    IDTRYAGAIN:                 10
    IDCONTINUE:                 11
}
