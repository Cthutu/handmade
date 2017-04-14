//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// Handmade Platform
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#pragma once

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// M A C R O S
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// G A M E   S E R V I C E S   I N T E R F A C E
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

struct GameOffscreenBuffer
{
    void*   memory;
    int     width;
    int     height;
    int     pitch;
};

struct GameSoundOutputBuffer
{
    int     samplesPerSecond;
    int     sampleCount;
    s16*    samples;
};

struct GameButtonState
{
    int     halfTransitionCount;
    b32     endedDown;
};

struct GameControllerInput
{
    b32     isAnalogue;

    f32     startX;
    f32     startY;

    f32     minX;
    f32     minY;

    f32     maxX;
    f32     maxY;

    f32     endX;
    f32     endY;

    union
    {
        GameButtonState buttons[6];
        struct
        {
            GameButtonState     up;
            GameButtonState     down;
            GameButtonState     left;
            GameButtonState     right;
            GameButtonState     leftShoulder;
            GameButtonState     rightShoulder;
        };
    };
};

struct GameInput
{
    GameControllerInput     controllers[4];
};

internal void gameUpdateAndRender(GameInput* input,
                                  GameOffscreenBuffer* buffer, 
                                  GameSoundOutputBuffer* soundBuffer);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// P L A T F O R M   S E R V I C E S   I N T E R F A C E
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

