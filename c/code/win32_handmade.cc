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


struct Win32OffscreenBuffer
{
    BITMAPINFO info;
    void* memory;
    int width;
    int height;
    int pitch;
};

global_variable bool gRunning = true;
global_variable Win32OffscreenBuffer gGlobalBackBuffer;

struct Win32WindowDimension
{
    int width;
    int height;
};

Win32WindowDimension Win32GetWindowDimension(HWND window)
{
    RECT clientRect;
    GetClientRect(window, &clientRect);
    return{
        clientRect.right - clientRect.left,
        clientRect.bottom - clientRect.top
    };
}

internal void RenderWeirdGradient(Win32OffscreenBuffer buffer, int blueOffset, int greenOffset)
{
    u8* row = (u8 *)buffer.memory;
    for (int y = 0; y < buffer.height; ++y)
    {
        u32* pixel = (u32 *)row;
        for (int x = 0; x < buffer.width; ++x)
        {
            // Pixel in memory: 00 RR GG BB
            u8 blue = (x + blueOffset);
            u8 green = (y + greenOffset);

            *pixel++ = (((u32)green << 8) | blue);
        }

        row += buffer.pitch;
    }
}

internal void Win32ResizeDIBSection(Win32OffscreenBuffer* buffer, int width, int height)
{
    if (buffer->memory)
    {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    buffer->width = width;
    buffer->height = height;
    int bytesPerPixel = 4;

    // Create the new DIBSection
    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = buffer->width;
    buffer->info.bmiHeader.biHeight = -buffer->height;
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;

    int bitmapMemorySize = (buffer->width * buffer->height) * bytesPerPixel;
    buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    buffer->pitch = width * bytesPerPixel;

}

internal void Win32DisplayBufferInWindow(HDC dc, int windowWidth, int windowHeight, 
                                         Win32OffscreenBuffer buffer)
{
    StretchDIBits(
        dc,
        0, 0, windowWidth, windowHeight,
        0, 0, buffer.width, buffer.height,
        buffer.memory,
        &buffer.info,
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

            Win32WindowDimension dimension = Win32GetWindowDimension(window);
            Win32DisplayBufferInWindow(dc, dimension.width, dimension.height, gGlobalBackBuffer);
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

    Win32ResizeDIBSection(&gGlobalBackBuffer, 1280, 720);

    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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
            HDC deviceContext = GetDC(window);

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

                RenderWeirdGradient(gGlobalBackBuffer, xOffset, yOffset);

                Win32WindowDimension dimension = Win32GetWindowDimension(window);
                Win32DisplayBufferInWindow(deviceContext, dimension.width, dimension.height, gGlobalBackBuffer);

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

