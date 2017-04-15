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

//  HANDMADE_INTERNAL:
//      0 - Build for public release.
//      1 - Build for developer only.
//
//  HANDMADE_SLOW:
//      0 - No slow code allowed!
//      1 - Slow code welcome.
//

#if HANDMADE_SLOW
#   define ASSERT(expression) if (!(expression)) { *(int *)0 = 0; }
#else
#   define ASSERT(...)
#endif

#define KB(x) (1024 * (u64)(x))
#define MB(x) (1024 * KB(x))
#define GB(x) (1024 * MB(x))
#define TB(x) (1024 * GB(x))

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
    i16*    samples;
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

struct GameMemory
{
    b32     isInitialised;

    u64     permanentStorageSize;
    void*   permanentStorage;           // Required to be cleared to zero at start up.

    u64     transientStorageSize;
    void*   transientStorage;           // Required to be cleared to zero at start up.
};


internal void gameUpdateAndRender(GameMemory* memory,
                                  GameInput* input,
                                  GameOffscreenBuffer* buffer, 
                                  GameSoundOutputBuffer* soundBuffer);

//
//
//

struct GameState
{
    int     toneHz;
    int     greenOffset;
    int     blueOffset;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// P L A T F O R M   S E R V I C E S   I N T E R F A C E
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

