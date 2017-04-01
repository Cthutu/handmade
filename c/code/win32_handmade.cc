//
// Handmade Hero
//

#include <Windows.h>
#include <stdint.h>
#include <Xinput.h>
#include <dsound.h>

#define internal static
#define local_persist static
#define global_variable static

//----------------------------------------------------------------------------------------------------------------------
// Basic types

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef int32_t b32;
typedef int64_t b64;

//----------------------------------------------------------------------------------------------------------------------
// Win32OffscreenBuffer

struct Win32OffscreenBuffer
{
    BITMAPINFO info;
    void* memory;
    int width;
    int height;
    int pitch;
};

//----------------------------------------------------------------------------------------------------------------------
// XInput API


#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(XInputGetStateFunc);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable XInputGetStateFunc* XInputGetState_ = XInputGetStateStub;

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(XInputSetStateFunc);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable XInputSetStateFunc* XInputSetState_ = XInputSetStateStub;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

internal void Win32LoadXInput()
{
    HMODULE xInputLibrary = LoadLibraryA("xinput1_4.dll");
    if (!xInputLibrary)
    {
        xInputLibrary = LoadLibraryA("xinput1_3.dll");
    }

    if (xInputLibrary)
    {
        XInputGetState = (XInputGetStateFunc*)GetProcAddress(xInputLibrary, "XInputGetState");
        if (!XInputGetState) XInputGetState = XInputGetStateStub;

        XInputSetState = (XInputSetStateFunc*)GetProcAddress(xInputLibrary, "XInputSetState");
        if (!XInputSetState) XInputSetState = XInputSetStateStub;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Sound management

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(DirectSoundCreateFunc);

internal void Win32InitDSound(HWND window, u32 samplesPerSec, u32 bufferSize)
{
    // Load the library
    HMODULE dSoundLibrary = LoadLibraryA("dsound.dll");
    if (dSoundLibrary)
    {
        // Get a DirectSound object
        DirectSoundCreateFunc* DirectSoundCreate = (DirectSoundCreateFunc*)GetProcAddress(dSoundLibrary, 
                                                                                          "DirectSoundCreate");
        LPDIRECTSOUND directSound;
        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &directSound, 0)))
        {
            WAVEFORMATEX waveFormat = {};
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = 2;
            waveFormat.wBitsPerSample = 16;
            waveFormat.nSamplesPerSec = samplesPerSec;
            waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize = 0;

            if (SUCCEEDED(directSound->SetCooperativeLevel(window, DSSCL_PRIORITY)))
            {
                // Create a primary buffer
                DSBUFFERDESC bufferDescription = { };
                bufferDescription.dwSize = sizeof(bufferDescription);
                bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

                LPDIRECTSOUNDBUFFER primaryBuffer;
                if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &primaryBuffer, 0)))
                {
                    if (SUCCEEDED(primaryBuffer->SetFormat(&waveFormat)))
                    {
                        DSBUFFERDESC bufferDescription = {};
                        bufferDescription.dwSize = sizeof(bufferDescription);
                        bufferDescription.dwFlags = 0;
                        bufferDescription.dwBufferBytes = bufferSize;
                        bufferDescription.lpwfxFormat = &waveFormat;

                        LPDIRECTSOUNDBUFFER secondaryBuffer;
                        if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &secondaryBuffer, 0)))
                        {
                            OutputDebugStringA("Created secondary sound buffer\n");
                        }
                    }
                    else
                    {
                        OutputDebugStringA("ERROR: Could not set format on primary buffer\n");
                    }
                }
                else
                {
                    OutputDebugStringA("ERROR: Could not create primary buffer");
                }
            }
            else
            {
                OutputDebugStringA("ERROR: Could not set cooperative level");
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Global state

global_variable bool gRunning = true;
global_variable Win32OffscreenBuffer gGlobalBackBuffer;

//----------------------------------------------------------------------------------------------------------------------
// Win32GetWindowDimension

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

//----------------------------------------------------------------------------------------------------------------------
// Rendering

internal void RenderWeirdGradient(Win32OffscreenBuffer* buffer, int blueOffset, int greenOffset)
{
    u8* row = (u8 *)buffer->memory;
    for (int y = 0; y < buffer->height; ++y)
    {
        u32* pixel = (u32 *)row;
        for (int x = 0; x < buffer->width; ++x)
        {
            // Pixel in memory: 00 RR GG BB
            u8 blue = (x + blueOffset);
            u8 green = (y + greenOffset);

            *pixel++ = (((u32)green << 8) | blue);
        }

        row += buffer->pitch;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Backbuffer creation

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
    buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    buffer->pitch = width * bytesPerPixel;

}

//----------------------------------------------------------------------------------------------------------------------
// Win32 rendering

internal void Win32DisplayBufferInWindow(Win32OffscreenBuffer* buffer, HDC dc, int windowWidth, int windowHeight)
{
    StretchDIBits(
        dc,
        0, 0, windowWidth, windowHeight,
        0, 0, buffer->width, buffer->height,
        buffer->memory,
        &buffer->info,
        DIB_RGB_COLORS,
        SRCCOPY);
}

//----------------------------------------------------------------------------------------------------------------------
// Win32 messages handler

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

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
        {
            u32 vkCode = (u32)wParam;
            bool wasDown = ((lParam & (1 << 30)) != 0);
            bool isDown = ((lParam & (1 << 31)) == 0);

            if (wasDown != isDown)
            {
                if (vkCode == 'W')
                {
                }
                else if (vkCode == 'A')
                {
                }
                else if (vkCode == 'S')
                {
                }
                else if (vkCode == 'D')
                {
                }
                else if (vkCode == 'Q')
                {
                }
                else if (vkCode == 'E')
                {
                }
                else if (vkCode == VK_UP)
                {
                }
                else if (vkCode == VK_LEFT)
                {
                }
                else if (vkCode == VK_DOWN)
                {
                }
                else if (vkCode == VK_RIGHT)
                {
                }
                else if (vkCode == VK_ESCAPE)
                {
                }
                else if (vkCode == VK_SPACE)
                {
                }
            }

            b64 altKeyWasDown = (lParam & (1ull << 29));
            if ((vkCode == VK_F4) && altKeyWasDown)
            {
                gRunning = false;
            }
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);
            Win32WindowDimension dimension = Win32GetWindowDimension(window);
            Win32DisplayBufferInWindow(&gGlobalBackBuffer, dc, dimension.width, dimension.height);
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

//----------------------------------------------------------------------------------------------------------------------
// Main loop

int CALLBACK WinMain(HINSTANCE inst, 
                     HINSTANCE prevInst, 
                     LPSTR cmdLine, 
                     int cmdShow)
{
    Win32LoadXInput();
 
    WNDCLASSA windowClass = {};

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

            Win32InitDSound(window, 48000, 48000*sizeof(s16)*2);

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

                for (DWORD controllerIndex = 0; controllerIndex <= XUSER_MAX_COUNT; ++controllerIndex)
                {
                    XINPUT_STATE controllerState;
                    if (XInputGetState(controllerIndex, &controllerState) == ERROR_SUCCESS)
                    {
                        // This controller is plugged in.
                        XINPUT_GAMEPAD* pad = &controllerState.Gamepad;

                        bool up = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
                        bool down = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
                        bool left = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
                        bool right = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
                        bool start = (pad->wButtons & XINPUT_GAMEPAD_START) != 0;
                        bool back = (pad->wButtons & XINPUT_GAMEPAD_BACK) != 0;
                        bool leftShoulder = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
                        bool rightShoulder = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
                        bool aButton = (pad->wButtons & XINPUT_GAMEPAD_A) != 0;
                        bool bButton = (pad->wButtons & XINPUT_GAMEPAD_B) != 0;
                        bool xButton = (pad->wButtons & XINPUT_GAMEPAD_X) != 0;
                        bool yButton = (pad->wButtons & XINPUT_GAMEPAD_Y) != 0;

                        s16 stickX = pad->sThumbLX;
                        s16 stickY = pad->sThumbLY;

                        xOffset += stickX >> 12;
                        yOffset += stickY >> 12;
                    }
                    else
                    {
                        // This controller is not available
                    }
                }

                RenderWeirdGradient(&gGlobalBackBuffer, xOffset, yOffset);

                Win32WindowDimension dimension = Win32GetWindowDimension(window);
                Win32DisplayBufferInWindow(&gGlobalBackBuffer, deviceContext, dimension.width, dimension.height);

                ++xOffset;
            }
        }
    }
    else
    {

    }

    return 0;
}

