-|                  |
 | Handmade Hero    |
 |                  |-

#import "Windows" as Win32

Win32OffscreenBuffer: {
    info            Win32.BITMAPINFO
    memory          ^void
    width           int
    height          int
    pitch           int
    bytesPerPixel   int
}

var 
    gRunning = true,
    gGlobalBackBuffer: Win32OffscreenBuffer


Win32GetWindowDimension: (window HWND) -> (width int, height int)
{
    var clientRect: RECT
    Win32.GetClientRect(window, ^clientRect)
    return (clientRect.right - clientRect.left, clientRect.bottom - clientRect.top)
}

RenderWeirdGradient: (buffer Win32OffscreenBuffer, blueOffset int, greenOffset int)
{
    let row = buffer.memory as ^u8
    loop y in (0, buffer.height)
    {
        unsafe
        {
            var pixel: ^u8 = row
            loop x in (0, buffer.width)
            {
                let blue = (x + blueOffset) as u8
                let green = (y + greenOffset) as u8

                pixel^= (green as u32) << 8 | (blue as u32)
                pixel += 1
            }

            row += buffer.pitch
        }
    }
}

Win32ResizeDIBSection: (buffer ^Win32OffscreenBuffer mutable, width int, height int)
{
    if buffer^.memory Win32.VirtualFree(gBitmapMemory, 0, Win32.MEM_RELEASE)

    buffer^.width = width;
    buffer^.height = height;

    buffer^.info.bmiheader = {
        biSize:         sizeof(buffer^.info.bmiHeader)
        biWidth:        buffer^.width
        biHeight:       -buffer^.height
        biPlanes:       1
        biBitCount:     32
        biCompression:  Win32.BI_RGB
    }

    let bitmapMemorySize = (buffer^.width * buffer^.height) * buffer^.bytesPerPixel
    buffer^.memory = Win32.VirtualAlloc(null, bitmapMemorySize, Win32.MEM_COMMIT, Win32.PAGE_READWRITE)
}

Win32UpdateWindow: (dc Win32.HDC, windowWidth int, windowHeight int, buffer Win32OffscreenBuffer, x int, y int, width int, height int)
{
    Win32.StretchDIBits(
        dc,
        0, 0, windowWidth, windowHeight
        x, y, buffer.width, buffer.height
        buffer.memory,
        ^buffer.info,
        Win32.DIB_RGB_COLORS,
        Win32.SRCCOPY)
}

main: ()
{
    Win32ResizeDIBSection(^gGlobalBackBuffer, 1280, 720)

    let windowClass: Win32.WNDCLASS = {
        style:          Win32.CS_HREDRAW | Win32.CS_VREDRAW
        lpfnWndProc:    (window Win32.HWND, message Win32.UINT, wParam Win32.WPARAM, lParam Win32.LPARAM) -> LRESULT
                        {
                            var result: Win32.LRESULT = 0

                            select message
                            {
                                on Win32.WM_SIZE
                                {
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
                                    let dc = Win32.BeginPaint(window, ^paint)

                                    let x = paint.rcPaint.left as int
                                    let y = paint.rcPaint.top as int
                                    let width = paint.rcPaint.right - x as int
                                    let height = paint.rcPaint.bottom - y as int

                                    let windowWidth, windowHeight = Win32GetWindowDimension(window)
                                    Win32UpdateWindow(dc, windowWidth, windowHeight, gGlobalBackBuffer, x, y, width, height)

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

                RenderWeirdGradient(gGlobalBackBuffer, xOffset, yOffset)

                let deviceContext = Win32.GetDC(window)
                let windowWidth, windowHeight = Win32GetWindowDimension(window)
                Win32UpdateWindow(deviceContext, windowWidth, windowHeight, gGlobalBackBuffer, 0, 0, windowWidth, windowHeight)
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

