//
// Handmade Hero
//

#include <Windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;


global_variable bool gRunning = true;

global_variable BITMAPINFO gBitmapInfo;
global_variable void* gBitmapMemory;
global_variable int gBitmapWidth;
global_variable int gBitmapHeight;
global_variable kBytesPerPixel = 4;

internal void RenderWeirdGradient(int blueOffset, int greenOffset)
{
    int width = gBitmapWidth;
    int height = gBitmapHeight;

    int pitch = width * kBytesPerPixel;
    u8* row = (u8 *)gBitmapMemory;
    for (int y = 0; y < height; ++y)
    {
        u32* pixel = (u32 *)row;
        for (int x = 0; x < width; ++x)
        {
            // Pixel in memory: 00 RR GG BB
            u8 blue = (x + blueOffset);
            u8 green = (y + greenOffset);

            *pixel++ = (((u32)green << 8) | blue);
        }

        row += pitch;
    }
}

internal void Win32ResizeDIBSection(int width, int height)
{
    if (gBitmapMemory)
    {
        VirtualFree(gBitmapMemory, 0, MEM_RELEASE);
    }

    gBitmapWidth = width;
    gBitmapHeight = height;

    // Create the new DIBSection
    gBitmapInfo.bmiHeader.biSize = sizeof(gBitmapInfo.bmiHeader);
    gBitmapInfo.bmiHeader.biWidth = gBitmapWidth;
    gBitmapInfo.bmiHeader.biHeight = -gBitmapHeight;
    gBitmapInfo.bmiHeader.biPlanes = 1;
    gBitmapInfo.bmiHeader.biBitCount = 32;
    gBitmapInfo.bmiHeader.biCompression = BI_RGB;

    int bitmapMemorySize = (gBitmapWidth * gBitmapHeight) * kBytesPerPixel;
    gBitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

}

internal void Win32UpdateWindow(HDC dc, RECT* clientRect, int x, int y, int width, int height)
{
    int windowWidth = clientRect->right - clientRect->left;
    int windowHeight = clientRect->bottom - clientRect->top;

    StretchDIBits(
        dc,
        0, 0, windowWidth, windowHeight,
        0, 0, gBitmapWidth, gBitmapHeight,
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

            RECT clientRect;
            GetClientRect(window, &clientRect);

            Win32UpdateWindow(dc, &clientRect, x, y, width, height);

            EndPaint(window, &paint);
        }
        break;

    default:
        {
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
        HWND window = CreateWindowEx(
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
        if (window)
        {
            int xOffset = 0;
            int yOffset = 0;

            gRunning = true;
            while(gRunning)
            {
                MSG message;
                while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
                {
                    if (message.message == WM_QUIT)
                    {
                        gRunning = false;
                    }

                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }

                RenderWeirdGradient(xOffset, yOffset);

                HDC deviceContext = GetDC(window);
                RECT clientRect;
                GetClientRect(window, &clientRect);
                int windowWidth = clientRect.right - clientRect.left;
                int windowHeight = clientRect.bottom - clientRect.top;
                Win32UpdateWindow(deviceContext, &clientRect, 0, 0, windowWidth, windowHeight);
                ReleaseDC(window, deviceContext);

                ++xOffset;
                yOffset += 2;
            }
        }
    }
    else
    {

    }

    return 0;
}

