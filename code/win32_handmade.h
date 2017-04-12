//----------------------------------------------------------------------------------------------------------------------
// Win32 Handmade declarations
//----------------------------------------------------------------------------------------------------------------------

#pragma once

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
// Win32GetWindowDimension

struct Win32WindowDimension
{
    int width;
    int height;
};

//----------------------------------------------------------------------------------------------------------------------
// Win32SoundOutput

struct Win32SoundOutput
{
    int samplesPerSecond;
    int toneHz;
    s16 toneVolume;
    u32 runningSampleIndex;
    int bytesPerSample;
    int bufferSize;
    f32 tSine;
    int latencySampleCount;
};

