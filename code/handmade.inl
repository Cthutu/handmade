//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// Handmade
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#include "handmade.h"

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



internal void gameUpdateAndRender(GameOffscreenBuffer* buffer, int blueOffset, int greenOffset)
{
    renderWeirdGradient(buffer, blueOffset, greenOffset);
}