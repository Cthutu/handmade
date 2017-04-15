//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// Handmade Hero
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// INDEX
//
//  COMMON      Common cross-platform declarations defined with platform-specific definitions.
//  GLOBALS     Global variables
//  INPUT       Input management
//  MAIN        Main loop
//  PLATFORM    Cross-platform code
//  RENDER      Rendering
//  SOUND       Sound management
//
//----------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------{COMMON}
//----------------------------------------------------------------------------------------------------------------------
// C O M M O N   D E F I N I T I O N S
//
// These declarations are common across platforms but their definitions are platform specific.
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include <math.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef int32_t     b32;
typedef int64_t     b64;

typedef float       f32;
typedef double      f64;

#define kPI 3.14159265359f

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// C R O S S - P L A T F O R M   C O D E
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#include "handmade.inl"

#include <Windows.h>
#include <dsound.h>
#include <malloc.h>
#include <stdio.h>
#include <Xinput.h>

#include "win32_handmade.h"

//-------------------------------------------------------------------------------------------------------------{GLOBALS}
//----------------------------------------------------------------------------------------------------------------------
// G L O B A L   S T A T E 
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

global_variable bool gRunning = true;
global_variable Win32OffscreenBuffer gGlobalBackBuffer;
global_variable LPDIRECTSOUNDBUFFER gSoundBuffer;

//---------------------------------------------------------------------------------------------------------------{INPUT}
//----------------------------------------------------------------------------------------------------------------------
// I N P U T   M A N A G E M E N T
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

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

internal void win32LoadXInput()
{
    HMODULE xInputLibrary = LoadLibraryA("xinput1_4.dll");
    if (!xInputLibrary)
    {
        xInputLibrary = LoadLibraryA("xinput_9_1_0.dll");
    }
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

internal void win32ProcessXInputDigitalButton(DWORD xInputButtonState, GameButtonState* oldState, DWORD buttonBit, GameButtonState* newState)
{
    newState->endedDown = (xInputButtonState & buttonBit) == buttonBit;
    newState->halfTransitionCount = (oldState->endedDown != newState->endedDown) ? 1 : 0;
}

//---------------------------------------------------------------------------------------------------------------{SOUND}
//----------------------------------------------------------------------------------------------------------------------
// S O U N D   M A N A G E M E N T
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(DirectSoundCreateFunc);

internal void win32InitDSound(HWND window, u32 samplesPerSec, u32 bufferSize)
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

                        if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &gSoundBuffer, 0)))
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

internal void win32ClearBuffer(Win32SoundOutput* soundOutput)
{
    void* region1 = nullptr;
    DWORD region1Size = 0;
    void* region2 = nullptr;
    DWORD region2Size = 0;

    if (SUCCEEDED(gSoundBuffer->Lock(0, soundOutput->bufferSize,
                                     &region1, &region1Size,
                                     &region2, &region2Size,
                                     0)))
    {
        i8* destSample = (i8 *)region1;
        for (DWORD byteIndex = 0; byteIndex < region1Size; ++byteIndex)
        {
            *destSample++ = 0;
        }

        destSample = (i8 *)region2;
        for (DWORD byteIndex = 0; byteIndex < region2Size; ++byteIndex)
        {
            *destSample++ = 0;
        }

        gSoundBuffer->Unlock(region1, region1Size, region2, region2Size);
    }
}

internal void win32FillSound(Win32SoundOutput* soundOutput, DWORD byteToLock, DWORD bytesToWrite, 
                    GameSoundOutputBuffer* sourceBuffer)
{
    void* region1 = nullptr;
    DWORD region1Size = 0;
    void* region2 = nullptr;
    DWORD region2Size = 0;

    if (SUCCEEDED(gSoundBuffer->Lock(byteToLock, bytesToWrite,
                                     &region1, &region1Size,
                                     &region2, &region2Size,
                                     0)))
    {
        // Region 1 fill
        DWORD region1SampleCount = region1Size / soundOutput->bytesPerSample;
        i16* destSample = (i16 *)region1;
        i16* srcSample = sourceBuffer->samples;
        for (DWORD sampleIndex = 0; sampleIndex < region1SampleCount; ++sampleIndex)
        {
            *destSample++ = *srcSample++;
            *destSample++ = *srcSample++;
            ++soundOutput->runningSampleIndex;
        }

        // Region 2 fill
        DWORD region2SampleCount = region2Size / soundOutput->bytesPerSample;
        destSample = (i16 *)region2;
        for (DWORD sampleIndex = 0; sampleIndex < region2SampleCount; ++sampleIndex)
        {
            *destSample++ = *srcSample++;
            *destSample++ = *srcSample++;
            ++soundOutput->runningSampleIndex;
        }

        gSoundBuffer->Unlock(region1, region1Size, region2, region2Size);
    }

}

//--------------------------------------------------------------------------------------------------------------{RENDER}
//----------------------------------------------------------------------------------------------------------------------
// R E N D E R I N G
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

Win32WindowDimension win32GetWindowDimension(HWND window)
{
    RECT clientRect;
    GetClientRect(window, &clientRect);
    return{
        clientRect.right - clientRect.left,
        clientRect.bottom - clientRect.top
    };
}

//----------------------------------------------------------------------------------------------------------------------
// Backbuffer creation

internal void win32ResizeDIBSection(Win32OffscreenBuffer* buffer, int width, int height)
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

internal void win32DisplayBufferInWindow(Win32OffscreenBuffer* buffer, HDC dc, int windowWidth, int windowHeight)
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

//----------------------------------------------------------------------------------------------------------------{MAIN}
//----------------------------------------------------------------------------------------------------------------------
// M A I N   L O O P
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK win32MainWindowCallback(HWND window, 
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
            Win32WindowDimension dimension = win32GetWindowDimension(window);
            win32DisplayBufferInWindow(&gGlobalBackBuffer, dc, dimension.width, dimension.height);
            EndPaint(window, &paint);
        }
        break;

    default:
        {
            result = DefWindowProcA(window, message, wParam, lParam);
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
    LARGE_INTEGER perfCounterFrequencyResult;
    QueryPerformanceFrequency(&perfCounterFrequencyResult);
    i64 perfCounterFrequency = perfCounterFrequencyResult.QuadPart;

    win32LoadXInput();
 
    WNDCLASSA windowClass = {};

    win32ResizeDIBSection(&gGlobalBackBuffer, 1280, 720);

    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = &win32MainWindowCallback;
    windowClass.hInstance = inst;
    windowClass.lpszClassName = "HandmadeHeroWindowClass";

    if (RegisterClassA(&windowClass))
    {
        HWND window = CreateWindowExA(
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

            // Graphics test
            int xOffset = 0;
            int yOffset = 0;

            //----------------------------------------------------------------------------------------------------------
            // Initialise sound

            Win32SoundOutput soundOutput = {};
            soundOutput.samplesPerSecond = 48000;
            soundOutput.toneHz = 256;
            soundOutput.toneVolume = 3000;
            soundOutput.bytesPerSample = sizeof(i16) * 2;
            soundOutput.bufferSize = soundOutput.samplesPerSecond * soundOutput.bytesPerSample;
            soundOutput.latencySampleCount = soundOutput.samplesPerSecond / 15;

            win32InitDSound(window, soundOutput.samplesPerSecond, soundOutput.bufferSize);
            win32ClearBuffer(&soundOutput);
            gSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

            i16* samples = (i16 *)VirtualAlloc(0, soundOutput.bufferSize,
                                               MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

            //----------------------------------------------------------------------------------------------------------
            // Initialise memory

#if HANDMADE_INTERNAL
            LPVOID baseAddress = (LPVOID)TB(2);
#else
            LPVOID baseAddress = 0;
#endif
            GameMemory gameMemory = {};
            gameMemory.permanentStorageSize = MB(64);
            gameMemory.transientStorageSize = GB(4);

            u64 totalSize = gameMemory.permanentStorageSize + gameMemory.transientStorageSize;

            gameMemory.permanentStorage = VirtualAlloc(baseAddress, totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            gameMemory.transientStorage = ((u8 *)gameMemory.permanentStorage) + gameMemory.permanentStorageSize;

            //----------------------------------------------------------------------------------------------------------
            // Initialise input

            GameInput input[2] = {};
            GameInput* newInput = &input[0];
            GameInput* oldInput = &input[1];

            //----------------------------------------------------------------------------------------------------------
            // Main loop

            if (samples && gameMemory.permanentStorage && gameMemory.transientStorage)
            {
                gRunning = true;

                LARGE_INTEGER lastCounter;
                QueryPerformanceCounter(&lastCounter);

                u64 lastCycleCount = __rdtsc();

                while (gRunning)
                {
                    MSG message;

                    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
                    {
                        if (message.message == WM_QUIT)
                        {
                            gRunning = false;
                        }

                        TranslateMessage(&message);
                        DispatchMessage(&message);
                    }

                    int maxControllerCount = XUSER_MAX_COUNT;
                    if (maxControllerCount > ARRAY_COUNT(newInput->controllers))
                    {
                        maxControllerCount = ARRAY_COUNT(newInput->controllers);
                    }

                    for (DWORD controllerIndex = 0; controllerIndex <= XUSER_MAX_COUNT; ++controllerIndex)
                    {
                        GameControllerInput* oldController = &oldInput->controllers[controllerIndex];
                        GameControllerInput* newController = &newInput->controllers[controllerIndex];

                        XINPUT_STATE controllerState;
                        if (XInputGetState(controllerIndex, &controllerState) == ERROR_SUCCESS)
                        {
                            // This controller is plugged in.
                            XINPUT_GAMEPAD* pad = &controllerState.Gamepad;

                            bool up = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
                            bool down = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
                            bool left = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
                            bool right = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

                            f32 stickX = pad->sThumbLX / ((pad->sThumbLX < 0) ? 32768.0f : 32767.0f);
                            f32 stickY = pad->sThumbLY / ((pad->sThumbLY < 0) ? 32768.0f : 32767.0f);

                            newController->isAnalogue = true;
                            newController->startX = oldController->endX;
                            newController->startY = oldController->endY;
                            newController->minX = newController->maxX = newController->endX = stickX;
                            newController->minY = newController->maxY = newController->endY = stickY;

                            win32ProcessXInputDigitalButton(pad->wButtons, &oldController->down, XINPUT_GAMEPAD_A, &newController->down);
                            win32ProcessXInputDigitalButton(pad->wButtons, &oldController->right, XINPUT_GAMEPAD_B, &newController->right);
                            win32ProcessXInputDigitalButton(pad->wButtons, &oldController->left, XINPUT_GAMEPAD_X, &newController->left);
                            win32ProcessXInputDigitalButton(pad->wButtons, &oldController->up, XINPUT_GAMEPAD_Y, &newController->up);
                            win32ProcessXInputDigitalButton(pad->wButtons, &oldController->leftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER, &newController->leftShoulder);
                            win32ProcessXInputDigitalButton(pad->wButtons, &oldController->rightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER, &newController->rightShoulder);

                            //                         bool start = (pad->wButtons & XINPUT_GAMEPAD_START) != 0;
                            //                         bool back = (pad->wButtons & XINPUT_GAMEPAD_BACK) != 0;
                        }
                        else
                        {
                            // This controller is not available
                        }
                    }

                    DWORD byteToLock = 0;
                    DWORD targetCursor = 0;
                    DWORD bytesToWrite = 0;
                    DWORD playCursor = 0;
                    DWORD writeCursor = 0;
                    bool soundIsValid = false;
                    if (SUCCEEDED(gSoundBuffer->GetCurrentPosition(&playCursor, &writeCursor)))
                    {
                        byteToLock = (soundOutput.runningSampleIndex * soundOutput.bytesPerSample)
                            % soundOutput.bufferSize;
                        targetCursor = (playCursor + (soundOutput.latencySampleCount * soundOutput.bytesPerSample))
                            % soundOutput.bufferSize;
                        if (byteToLock > targetCursor)
                        {
                            // Case 1:
                            // |********P-------W********|
                            bytesToWrite = soundOutput.bufferSize - byteToLock;
                            bytesToWrite += targetCursor;
                        }
                        else
                        {
                            // Case 2:
                            // |--------W*******P--------|
                            bytesToWrite = targetCursor - byteToLock;
                        }

                        soundIsValid = true;
                    }

                    GameSoundOutputBuffer soundBuffer = {};
                    soundBuffer.samplesPerSecond = soundOutput.samplesPerSecond;
                    soundBuffer.sampleCount = bytesToWrite / soundOutput.bytesPerSample;
                    soundBuffer.samples = samples;

                    GameOffscreenBuffer buffer = {};
                    buffer.memory = gGlobalBackBuffer.memory;
                    buffer.width = gGlobalBackBuffer.width;
                    buffer.height = gGlobalBackBuffer.height;
                    buffer.pitch = gGlobalBackBuffer.pitch;

                    gameUpdateAndRender(&gameMemory, newInput, &buffer, &soundBuffer);

                    // DSound output test
                    if (soundIsValid)
                    {
                        win32FillSound(&soundOutput, byteToLock, bytesToWrite, &soundBuffer);
                    }

                    Win32WindowDimension dimension = win32GetWindowDimension(window);
                    win32DisplayBufferInWindow(&gGlobalBackBuffer, deviceContext, dimension.width, dimension.height);

                    //
                    // Update clock
                    //

                    u64 endCycleCount = __rdtsc();

                    LARGE_INTEGER endCounter;
                    QueryPerformanceCounter(&endCounter);

                    // Display value here
                    u64 cyclesElapsed = endCycleCount - lastCycleCount;
                    i64 counterElapsed = endCounter.QuadPart - lastCounter.QuadPart;
                    f64 msPerFrame = ((1000.0f * counterElapsed) / (f64)perfCounterFrequency);
                    f64 fps = (f64)perfCounterFrequency / (f64)counterElapsed;
                    f64 mcpf = (f64)cyclesElapsed / (1000.f * 1000.f);

                    //                 char buffer[256];
                    //                 sprintf(buffer, "%fms/f, %ff/s, %fMc/f\n", msPerFrame, fps, mcpf);
                    //                 OutputDebugStringA(buffer);

                    lastCounter = endCounter;
                    lastCycleCount = endCycleCount;

                    GameInput* t = newInput;
                    newInput = oldInput;
                    oldInput = t;
                } // while
            } // if have memory
            else
            {
                // Out of memory!
            }

            //----------------------------------------------------------------------------------------------------------

        } // if (window)
        else
        {
            // No Window!
        }
    }
    else
    {

    }

    return 0;
}

