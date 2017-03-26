//
// Handmade Hero
//

#include <Windows.h>

#define internal static
#define local_persist static
#define global_variable static


global_variable bool gRunning = true;

global_variable BITMAPINFO gBitmapInfo;
global_variable void* gBitmapMemory;
global_variable HBITMAP gBitmapHandle;
global_variable HDC gBitmapDeviceContext;

internal void Win32ResizeDIBSection(int width, int height)
{
    // Free the old DIBSection
    if (gBitmapHandle)
    {
        DeleteObject(gBitmapHandle);
    }

    if (!gBitmapDeviceContext)
    {
        gBitmapDeviceContext = CreateCompatibleDC(0);
    }

    // Create the new DIBSection
    gBitmapInfo.bmiHeader.biSize = sizeof(gBitmapInfo.bmiHeader);
    gBitmapInfo.bmiHeader.biWidth = width;
    gBitmapInfo.bmiHeader.biHeight = height;
    gBitmapInfo.bmiHeader.biPlanes = 1;
    gBitmapInfo.bmiHeader.biBitCount = 32;
    gBitmapInfo.bmiHeader.biCompression = BI_RGB;

    gBitmapHandle = CreateDIBSection(
        gBitmapDeviceContext,
        &gBitmapInfo,
        DIB_RGB_COLORS,
        &gBitmapMemory,
        NULL, 0);
}

internal void Win32UpdateWindow(HDC dc, int x, int y, int width, int height)
{
    StretchDIBits(
        dc,
        x, y, width, height,        // Destination
        x, y, width, height,        // Source
        gBitmapMemory,
        &gBitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND window,
                                         UINT message,
                                         WPARAM wParam,
                                         LPARAM lParam)
{
    LRESULT result = 0;

    switch(message)
    {
    case WM_SIZE:
        {
            RECT clientRect;
            GetClientRect(window, &clientRect);
            int width = (int)(clientRect.right - clientRect.left);
            int height = (int)(clientRect.bottom - clientRect.top);
            Win32ResizeDIBSection(width, height);
            OutputDebugStringA("WM_SIZE\n");
        }
        break;

    case WM_CLOSE:
        {
            gRunning = false;
        }
        break;

    case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }
        break;

    case WM_DESTROY:
        {
            gRunning = false;
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);

            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;
            Win32UpdateWindow(dc, x, y, width, height);

            EndPaint(window, &paint);
        }
        break;

    default:
        {
            OutputDebugStringA("default\n");
            result = DefWindowProc(window, message, wParam, lParam);
        }
        break;
    }

    return result;
}

int CALLBACK WinMain(HINSTANCE inst, 
                     HINSTANCE prevInst, 
                     LPSTR cmdLine, 
                     int cmdShow)
{
    WNDCLASS windowClass = {};

    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = &Win32MainWindowCallback;
    windowClass.hInstance = inst;
    windowClass.lpszClassName = "HandmadeHeroWindowClass";

    if (RegisterClass(&windowClass))
    {
        HWND windowHandle = CreateWindowEx(
            0,
            windowClass.lpszClassName,
            "Handmade Hero",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            inst,
            0);
        if (windowHandle)
        {
            MSG message;
            gRunning = true;
            while(gRunning)
            {
                BOOL messageResult = GetMessage(&message, 0, 0, 0);
                if (messageResult > 0)
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
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

    return 0;
}

