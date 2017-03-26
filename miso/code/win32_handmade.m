-|                  |
 | Handmade Hero    |
 |                  |-

#import "Windows" as Win32

var gRunning = true

var gBitmapInfo: Win32.BITMAPINFO
var gBitmapMemory: ^void
var gBitmapWidth, gBitmapHeight: int
kBytesPerPixel: 4

RenderWeirdGradient: (blueOffset int, greenOffset int)
{
    let width = gBitmapWidth
    let height = gBitmapHeight
    let pitch = width * kBytesPerPixel

    let row = gBitmapMemory as ^u8
    loop y in (0, height)
    {
        unsafe
        {
            var pixel: ^u8 = row
            loop x in (0, width)
            {
                let blue = (x + blueOffset) as u8
                let green = (y + greenOffset) as u8

                pixel^= (green as u32) << 8 | (blue as u32)
                pixel += 1
            }

            row += pitch
        }
    }
}

Win32ResizeDIBSection: (width int, height int)
{
    if gBitmapMemory Win32.VirtualFree(gBitmapMemory, 0, Win32.MEM_RELEASE)

    gBitmapWidth = width;
    gBitmapHeight = height;

    gBitmapInfo.bmiheader = {
        biSize:         sizeof(gBitmapInfo.bmiHeader)
        biWidth:        width
        biHeight:       -height
        biPlanes:       1
        biBitCount:     32
        biCompression:  Win32.BI_RGB
    }

    let bitmapMemorySize = (width * height) * kBytesPerPixel
    gBitmapMemory = Win32.VirtualAlloc(null, bitmapMemorySize, Win32.MEM_COMMIT, Win32.PAGE_READWRITE)
}

Win32UpdateWindow: (dc Win32.HDC, clientRect ^RECT, x int, y int, width int, height int)
{
    let windowWidth = (clientRect.right - clientRect.left) as int
    let windowHeight = (clientRect.bottom - clientRect.top) as int

    Win32.StretchDIBits(
        dc,
        0, 0, windowWidth, windowHeight
        x, y, gBitmapWidth, gBitmapHeight
        gBitmapMemory,
        ^gBitmapInfo,
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
        let window = Win32.CreateWindowEx(
            lpszClassName:      windowClassName.lpszClassName,
            lpszWindowName:     "Handmade Hero",
            dwStyle:            Win32.WS_OVERLAPPEDWINDOW | Win32.WS_VISIBLE,
            x:                  CW_USEDEFAULT,
            y:                  CW_USEDEFAULT,
            nWidth:             CW_USEDEFAULT,
            nHeight:            CW_USEDEFAULT,
            hInstance:          windowClassName.hInstance
        )

        if (window)
        {
            var xOffset, yOffset: int
            gRunning = true
            
            while(gRunning) {
                var message: Win32.MSG
                while(Win32.PeekMessage(^message, 0, 0, 0, Win32.PM_REMOVE))
                {
                    gRunning = false if (message.message == Win32.WM_QUIT)
                    Win32.TranslateMessage(^message)
                    Win32.DispatchMessage(^message)
                }

                RenderWeirdGradient(xOffset, yOffset)

                let deviceContext = Win32.GetDC(window)
                var clientRect: Win32.RECT
                Win32.GetClientRect(window, ^clientRect)
                let windowWidth = (clientRect.right - clientRect.left) as int
                let windowHeight = (clientRect.bottom - clientRect.top) as int
                Win32UpdateWindow(deviceContext, ^clientRect, 0, 0, windowWidth, windowHeight)
                Win32.ReleaseDC(window, deviceContext)

                xOffset += 1
                yOffset += 2
            }
        }
    }
    else
    {

    }

    return 0
}

