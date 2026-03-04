
#pragma once

#include "../include/video.h"

bool palette_matches_stream(
    const uint32_t *palette,
    const uint8_t  *stream,
    size_t          num_colors
)
{
    size_t i;
    size_t base = VIDEO_STREAM_HEADER_SIZE;

    for (i = 0; i < num_colors; i++) {
        size_t off = base + (i * PALETTE_BYTES_PER_COLOR);

        uint8_t r = stream[off + 0];
        uint8_t g = stream[off + 1];
        uint8_t b = stream[off + 2];

        uint32_t expected =
            ((uint32_t)r << 16) |
            ((uint32_t)g << 8)  |
            ((uint32_t)b);

        if (palette[i] != expected)
            return false;
    }

    return true;
}



void printPalette(const uint32_t* palette, const uint16_t num_colors) {

    for (int i = 0; i < num_colors; i++) { 

        printf("%06x ", palette[i]);
        if ((i+1) % 16 == 0) { 
            printf("\n");
        }
    }
}
