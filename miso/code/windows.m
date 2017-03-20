-- Windows header file

export
{
    -- Handles
    HANDLE: ^void
    HINSTANCE: HANDLE
    HMODULE: HINSTANCE
    HWND: HANDLE
    HDC: HANDLE
    HCURSOR: HICON
    HICON: HANDLE
    HBRUSH: HANDLE

    -- Integers & pointers
    UINT: u32
    WORD: u16
    DWORD: u32
    LONG: s32
    WPARAM: UINT_PTR
    LPARAM: LONG_PTR
    UINT_PTR: s64
    LONG_PTR: s64
    LRESULT: LONG_PTR
    TRUE: 1
    FALSE: 0
    ATOM: WORD
    LPVOID: ^void
    BOOL: int

    -- Strings
    LPSTR: ^CHAR
    LPCSTR: ^CHAR
    CHAR: char
    LPWSTR: ^WCHAR
    LPCWSTR: ^WCHAR
    WCHAR: ^rune

    -- Callbacks
    WNDPROC: (hwnd HWND, uMsg UINT, wParam WPARAM, lParam LPARAM) -> LRESULT

    -- Structures
    WNDCLASSA: {
        style           UINT
        lpfnWndProc     WNDPROC
        cbClsExtra      s32
        cbWndExtra      s32
        hInstance       HINSTANCE
        hIcon           HICON
        hCursor         HCURSOR
        hbrBackground   HBRUSH
        lpszMenuName    LPCSTR
        lpszClassName   LPCSTR
    }
    WNDCLASSW: {
        style           UINT
        lpfnWndProc     WNDPROC
        cbClsExtra      s32
        cbWndExtra      s32
        hInstance       HINSTANCE
        hIcon           HICON
        hCursor         HCURSOR
        hbrBackground   HBRUSH
        lpszMenuName    LPCWSTR
        lpszClassName   LPCWSTR
    }
    WNDCLASS: WNDCLASSA

    MSG: {
        hwnd            HWND
        message         UINT
        wParam          WPARAM
        lParam          LPARAM
        time            DWORD
        pt              POINT
    }
    LPMSG: ^MSG

    POINT: {
        x               LONG
        y               LONG
    }

    RECT: {
        left            LONG
        top             LONG
        right           LONG
        bottom          LONG
    }

    PAINTSTRUCT: {
        hdc             HDC
        fErase          BOOL
        rcPaint         RECT
        fRestore        BOOL
        fIncUpdate      BOOL
        rgbReserved     [32]BYTE
    }

    LPPAINTSTRUCT: ^PAINTSTRUCT

    -- Unicode KERNEL32 functions
    foreign(kernel32) GetModuleHandleW: (lpModuleName LPCWSTR) -> HMODULE

    -- ASCII KERNEL32 functions
    foreign(kernel32) GetModuleHandleA: (lpModuleName LPCSTR) -> HMODULE

    -- Default versions
    GetModuleHandle:            GetModuleHandleA


    -- USER32 functions
    foreign(user32) DefWindowProc(hWnd HWND, msg UINT, wParam WPARAM, lParam LPARAM) -> LRESULT
    foreign(user32) GetMessage(lpMsg LPMSG, hWnd HWND, wMsgFilterMin UINT, wMsgFilterMax UINT) -> BOOL
    foreign(user32) TranslateMessage(lpMsg LPMSG) -> BOOL
    foreign(user32) DispatchMessage(lpMsg LPMSG) -> LRESULT
    foreign(user32) BeginPaint(hwnd HWND, lpPaint LPPAINTSTRUCT) -> HDC
    foreign(user32) EndPaint(hwnd HWND, lpPaint LPPAINTSTRUCT) -> BOOL

    -- Unicode version USER32 functions
    foreign(user32) MessageBoxW: (hWnd HWND, lpText LPCWSTR, lpCaption LPCWSTR, uType UINT) -> int
    foreign(user32) RegisterClassW: (lpWndClass ^WNDCLASSW) -> ATOM
    foreign(user32) CreateWindowExW: (
                                        dwExStyle       DWORD,
                                        lpszClassName   LPCWSTR,
                                        lpszWindowName  LPCWSTR,
                                        dwStyle         DWORD,
                                        x               int,
                                        y               int,
                                        nWidth          int,
                                        nHeight         int,
                                        hWndParent      HWND,
                                        hMenu           HMENU,
                                        hInstance       HINSTANCE,
                                        lpParam         LPVOID
                                    ) -> HWND

    -- ASCII version USER32 functions
    foreign(user32) MessageBoxA: (hWnd HWND, lpText LPCSTR, lpCaption LPCSTR, uType UINT) -> int
    foreign(user32) RegisterClassA: (lpWndClass ^WNDCLASSA) -> ATOM
    foreign(user32) CreateWindowExA: (
                                        dwExStyle       DWORD,
                                        lpszClassName   LPCSTR,
                                        lpszWindowName  LPCSTR,
                                        dwStyle         DWORD,
                                        x               int,
                                        y               int,
                                        nWidth          int,
                                        nHeight         int,
                                        hWndParent      HWND,
                                        hMenu           HMENU,
                                        hInstance       HINSTANCE,
                                        lpParam         LPVOID
                                    ) -> HWND

    -- Default versions
    MessageBox:                 MessageBoxA
    RegisterClass:              RegisterClassA
    CreateWindow:               CreateWindowA

    -- GDI32 functions
    foreign(gdi32) PatBlt(hdc HDC, nXLeft int, nYLeft int, nWidth int, nHeight int, dwRop DWORD) -> BOOL

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

    CS_VREDRAW:                 0x00000001
    CS_HREDRAW:                 0x00000002
    CS_DBLCLK:                  0x00000008
    CS_OWNDC:                   0x00000020
    CS_CLASSDC:                 0x00000040
    CS_PARENTDC:                0x00000080
    CS_SAVEBITS:                0x00000800
    CS_BYTEALIGNCLIENT:         0x00001000
    CS_BYTEALIGNWINDOW:         0x00002000
    CS_GLOBALCLASS:             0x00004000
    CS_DROPSHADOW:              0x00020000
    CS_NOCLOSE:                 0x00000200

    CW_USEDEFAULT:              (0x80000000 as int)

    WS_OVERLAPPED:              0x00000000
    WS_TILED:                   0x00000000
    WS_MAXIMIZEBOX:             0x00010000
    WS_TABSTOP:                 0x00010000
    WS_GROUP:                   0x00020000
    WS_MINIMIZEBOX:             0x00020000
    WS_SIZEBOX:                 0x00040000
    WS_THICKFRAME:              0x00040000
    WS_SYSMENU:                 0x00080000
    WS_HSCROLL:                 0x00100000
    WS_VSCROLL:                 0x00200000
    WS_DLGFRAME:                0x00400000
    WS_BORDER:                  0x00800000
    WS_CAPTION:                 0x00c00000
    WS_MAXIMIZE:                0x01000000
    WS_CLIPCHILDREN:            0x02000000
    WS_CLIPSIBLINGS:            0x04000000
    WS_DISABLED:                0x08000000
    WS_VISIBLE:                 0x10000000
    WS_ICONIC:                  0x20000000
    WS_CHILD:                   0x40000000
    WS_CHILDWINDOW:             0x40000000
    WS_MINIMIZE:                0x20000000
    WS_POPUP:                   0x80000000
    WS_OVERLAPPEDWINDOW:        (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
    WS_POPUPWINDOW:             (WS_POPUP | WS_BORDER | WS_SYSMENU)
    WS_TILEDWINDOW:             (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)

    WS_EX_LEFT:                 0x00000000
    WS_EX_LTRREADING:           0x00000000
    WS_EX_RIGHTSCROLLBAR:       0x00000000
    WS_EX_DLGMODALFRAME:        0x00000001
    WS_EX_NOPARENTNOTIFY:       0x00000004
    WS_EX_TOPMOST:              0x00000008
    WS_EX_ACCEPTFILES:          0x00000010
    WS_EX_TRANSPARENT:          0x00000020
    WS_EX_MDICHILD:             0x00000040
    WS_EX_TOOLWINDOW:           0x00000080
    WS_EX_WINDOWEDGE:           0x00000100
    WS_EX_CLIENTEDGE:           0x00000200
    WS_EX_CONTEXTHELP:          0x00000400
    WS_EX_RIGHT:                0x00001000
    WS_EX_RTLREADING:           0x00002000
    WS_EX_LEFTSCROLLBAR:        0x00004000
    WS_EX_CONTROLPARENT:        0x00010000
    WS_EX_STATICEDGE:           0x00020000
    WS_EX_APPWINDOW:            0x00040000
    WS_EX_LAYERED:              0x00080000
    WS_EX_NOINHERITLAYOUT:      0x00100000
    WS_EX_NOREDIRECTIONBITMAP:  0x00200000
    WS_EX_LAYOUTRTL:            0x00400000
    WS_EX_COMPOSITED:           0x02000000
    WS_EX_NOACTIVATE:           0x08000000
    WS_EX_OVERLAPPEDWINDOW:     (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
    WS_EX_PALETTEWINDOW:        (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)

    -- Windows messages and their Constants
    WM_DESTROY:                 0x0002
    WM_SIZE:                    0x0005
    WM_PAINT:                   0x000f
    WM_CLOSE:                   0x0010
    WM_ACTIVATEAPP:             0x001c

    SIZE_RESTORED:              0
    SIZE_MINIMIZED:             1
    SIZE_MAXIMIZED:             2
    SIZE_MAXSHOW:               3
    SIZE_MAXHIDE:               4

    PATCOPY:                    0x00f00021 as DWORD
    PATINVERT:                  0x005A0049 as DWORD
    DSTINVERT:                  0x00550009 as DWORD
    BLACKNESS:                  0x00000042 as DWORD
    WHITENESS:                  0x00ff0062 as DWORD
}
