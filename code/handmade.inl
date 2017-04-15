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
    i16 toneVolume = 3000;
    i16* sampleOut = soundBuffer->samples;
    int wavePeriod = soundBuffer->samplesPerSecond / toneHz;

    for (int sampleIndex = 0; sampleIndex < soundBuffer->sampleCount; ++sampleIndex)
    {
        f32 sineValue = sinf(tSine);
        i16 sampleValue = (i16)(sineValue * toneVolume);
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

internal void gameUpdateAndRender(GameMemory* memory,
                                  GameInput* input, 
                                  GameOffscreenBuffer* buffer,
                                  GameSoundOutputBuffer* soundBuffer)
{
    ASSERT(sizeof(GameState) <= memory->permanentStorageSize);

    GameState* gameState = (GameState *)memory->permanentStorage;
    if (!memory->isInitialised)
    {
        gameState->toneHz = 256;
        memory->isInitialised = true;
    }

    GameControllerInput* input0 = &input->controllers[0];
    if (input0->isAnalogue)
    {
        // Use analogue movement tuning
        gameState->blueOffset += (int)(4.0f * input0->endX);
        gameState->toneHz = 256 + (int)(128.0f * input0->endY);
    }
    else
    {
        // Use digital movement tuning
    }

    if (input0->down.endedDown)
    {
        gameState->greenOffset += 1;
    }

    gameOutputSound(soundBuffer, gameState->toneHz);
    renderWeirdGradient(buffer, gameState->blueOffset, gameState->greenOffset);
}