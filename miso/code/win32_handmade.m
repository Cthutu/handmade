-|                  |
 | Handmade Hero    |
 |                  |-

#import "Windows" as Win32

var gRunning = true
var gBitmapInfo: Win32.BITMAPINFO
var gBitmapMemory: ^void
var gBitmapHandle: Win32.HBITMAP
var gBitmapDeviceContext: Win32.HDC

Win32ResizeDIBSection: (width int, height int)
{
    if gBitmapHandle Win32.DeleteObject(gBitMapHandle)
    if !gBitmapDeviceContext gBitmapDeviceContext = Win32.CreateCompatibleDC(0)

    gBitmapInfo.bmiheader = {
        biSize:         sizeof(gBitmapInfo.bmiHeader)
        biWidth:        width
        biHeight:       height
        biPlanes:       1
        biBitCount:     32
        biCompression:  Win32.BI_RGB
    }

    gBitmapHandle = Win32.CreateDIBSection(
        hdc:        gBitmapDeviceContext,
        pbmi:       ^gBitmapInfo,
        iUsage:     DIB_RGB_COLORS,
        ppvBits:    ^gBitmapMemory,
        hSection:   null,
        dwOffset:   0)
}

Win32UpdateWindow: (dc Win32.HDC, x int, y int, width int, height int)
{
    Win32.StretchDIBits(
        dc,
        x, y, width, height,
        x, y, width, height,
        0, 0,
        Win32.DIB_RGB_COLORS,
        Win32.SRCCOPY)
}

main: ()
{
    let windowClass: Win32.WNDCLASS = {
        style:          Win32.CS_OWNDC | Win32.CS_HREDRAW | Win32.CS_VREDRAW
        lpfnWndProc:    (window Win32.HWND, message Win32.UINT, wParam Win32.WPARAM, lParam Win32.LPARAM) -> LRESULT
                        {
                            var result: Win32.LRESULT = 0

                            select message
                            {
                                on Win32.WM_SIZE
                                {
                                    var clientRect: Win32.RECT
                                    Win32.GetClientRect(window, ^clientRect)
                                    let width = (clientRect.right - clientRect.left) as int
                                    let height = (clientRect.bottom - clientRect.top) as int
                                    Win32ResizeDIBSection(width, height);
                                }

                                on Win32.WM_CLOSE
                                {
                                    gRunning = false
                                }

                                on Win32.WM_ACTIVATEAPP
                                {

                                }

                                on Win32.WM_DESTROY
                                {
                                    gRunning = false
                                }

                                on Win32.WM_PAINT
                                {
                                    var paint: Win32.PAINTSTRUCT
                                    let deviceContext = Win32.BeginPaint(window, ^paint)

                                    let x = paint.rcPaint.left as int
                                    let y = paint.rcPaint.top as int
                                    let width = paint.rcPaint.right - x as int
                                    let height = paint.rcPaint.bottom - y as int
                                    Win32UpdateWindow(x, y, width, height)

                                    Win32.EndPaint(window, ^paint)
                                }

                                default
                                {
                                    result = DefWindowProc(window, message, wParam, lParam)
                                }
                            }
                        }
        hInstance:      Win32.GetModuleHandle(null)
        lpszClassName:  "HandmadeHeroWindowClass"
    }

    if Win32.RegisterClass(^windowClass)
    {
        let windowHandle = Win32.CreateWindowEx(
            lpszClassName:      windowClassName.lpszClassName,
            lpszWindowName:     "Handmade Hero",
            dwStyle:            Win32.WS_OVERLAPPEDWINDOW | Win32.WS_VISIBLE,
            x:                  CW_USEDEFAULT,
            y:                  CW_USEDEFAULT,
            nWidth:             CW_USEDEFAULT,
            nHeight:            CW_USEDEFAULT,
            hInstance:          windowClassName.hInstance
        )

        if (windowHandle)
        {
            var message: Win32.MSG
            gRunning = true
            while(gRunning) {
                let messageResult = Win32.GetMessage(^message, 0, 0, 0)
                if messageResult > 0
                {
                    Win32.TranslateMessage(^message)
                    Win32.DispatchMessage(^message)
                }
                else
                {
                    break;
                }
            }
        }
    }
    else
    {

    }

    return 0
}

