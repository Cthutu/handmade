-|                  |
 | Handmade Hero    |
 |                  |-

#import "Windows" as Win32

main: ()
{
    let windowClass: Win32.WNDCLASS = {
        style:          Win32.CS_OWNDC | Win32.CS_HREDRAW | Win32.CS_VREDRAW
        lpfnWndProc:    (window HWND, message UINT, wParam WPARAM, lParam LPARAM) -> LRESULT
                        {
                            var result: LRESULT = 0

                            select message
                            {
                                on Win32.WM_SIZE
                                {

                                }

                                on Win32.WM_DESTROY
                                {

                                }

                                on Win32.WM_ACTIVATEAPP
                                {

                                }

                                on Win32.WM_PAINT
                                {
                                    var paint: Win32.PAINTSTRUCT
                                    let deviceContext = Win32.BeginPaint(window, ^paint)
                                    let x = paint.rcPaint.left as int
                                    let y = paint.rcPaint.top as int
                                    let width = paint.rcPaint.right - x as int
                                    let height = paint.rcPaint.bottom - y as int

                                    var operation = Win32.WHITENESS @static
                                    Win32.PatBlt(dc, x, y, width, height, operation)
                                    operation = Win32.BLACKNESS if operation == Win32.WHITENESS else Win32.WHITENESS
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
            loop {
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

