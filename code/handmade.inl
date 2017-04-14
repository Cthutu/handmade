//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// Handmade
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#include "handmade.h"

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// A U D I O
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void gameOutputSound(GameSoundOutputBuffer* soundBuffer, int toneHz)
{
    local_persist f32 tSine;
    s16 toneVolume = 3000;
    s16* sampleOut = soundBuffer->samples;
    int wavePeriod = soundBuffer->samplesPerSecond / toneHz;

    for (int sampleIndex = 0; sampleIndex < soundBuffer->sampleCount; ++sampleIndex)
    {
        f32 sineValue = sinf(tSine);
        s16 sampleValue = (s16)(sineValue * toneVolume);
        *sampleOut++ = sampleValue;
        *sampleOut++ = sampleValue;

        tSine += 2.0f * kPI / wavePeriod;
    }

    
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// R E N D E R I N G
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

internal void renderWeirdGradient(GameOffscreenBuffer* buffer, int blueOffset, int greenOffset)
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
//----------------------------------------------------------------------------------------------------------------------
// M A I N   L O O P 
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

internal void gameUpdateAndRender(GameInput* input, 
                                  GameOffscreenBuffer* buffer,
                                  GameSoundOutputBuffer* soundBuffer)
{
    local_persist int blueOffset = 0;
    local_persist int greenOffset = 0;
    local_persist int toneHz = 256;

    GameControllerInput* input0 = &input->controllers[0];
    if (input0->isAnalogue)
    {
        // Use analogue movement tuning
        blueOffset += (int)(4.0f * input0->endX);
        toneHz = 256 + (int)(128.0f * input0->endY);
    }
    else
    {
        // Use digital movement tuning
    }

    if (input0->down.endedDown)
    {
        greenOffset += 1;
    }

    gameOutputSound(soundBuffer, toneHz);
    renderWeirdGradient(buffer, blueOffset, greenOffset);
}