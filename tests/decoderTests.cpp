

// Headers 
#include "CppUTest/TestHarness.h" 



#include "../include/video.h"
#include "../include/video_fsm.h"
#include "helper.cpp"
#include <iostream>
#include <stdio.h>

TEST_GROUP(parseHeaderTests) {

    void setup() { 


    }

    void teardown() { 


    }


};


// Happy path
TEST(parseHeaderTests, parseStreamHeaderSuccess)  {


    const uint8_t header[] =  {
        0x56, 0x43, 
        0x00, 0x6C, 
        0x00, 0x7A, 
        0x01, 0x00, 
        0xFF
    };

    video_handler_t video; 
    video_init(&video, header, sizeof(header));

    // Read file format bytes 
    // Read width and height bytes 
    // Get number of colors 
    // Get codec flags 

    parse_header_status_t status = parse_stream_header(&video);

    CHECK_EQUAL(HDR_OK, status);
    CHECK_EQUAL(0x5643, video.config.file_format);
    CHECK_EQUAL(108, video.config.width);
    CHECK_EQUAL(122, video.config.height);
    CHECK_EQUAL(256, video.config.num_colors);
    CHECK_EQUAL(0xFF, video.config.codec_flags);
   
}


TEST(parseHeaderTests, invalidStreamLength) { 

    
    const uint8_t header[] =  {
    };

    video_handler_t video; 
    video_init(&video, header, sizeof(header));

    parse_header_status_t status = parse_stream_header(&video);
    
    CHECK_EQUAL(HDR_INCOMPLETE, status);

}


TEST(parseHeaderTests, invalidFileID) { 


     const uint8_t header[] =  {
        0x56, 0x42,  // 0x5642 != 0x5643 ID does not match
        0x00, 0x80, // Width  = 128
        0x00, 0x80, // Height = 128
        0x01, 0x00, 
        0xFF
    };

    video_handler_t video; 
    video_init(&video, header, sizeof(header));


    parse_header_status_t status = parse_stream_header(&video);

    CHECK_EQUAL(HDR_ERR_INVALID_ID, status);

}


TEST(parseHeaderTests, emptyDimension) { 

    const uint8_t header[] =  {
        0x56, 0x43, 
        0x00, 0x00, // Width = 0
        0x00, 0x7A, 
        0x01, 0x00, 
        0xFF
    };


    video_handler_t video; 
    video_init(&video, header, sizeof(header));

    parse_header_status_t status = parse_stream_header(&video);

    CHECK_EQUAL(HDR_ERR_DIM_ZERO, status);
}



TEST(parseHeaderTests, dimensionExceedsMax) { 

     const uint8_t header[] =  {
        0x56, 0x43, 
        0x00, 0x80, // Width  = 128
        0x00, 0x81, // Height = 129 invalid!
        0x01, 0x00, 
        0xFF
    };

    video_handler_t video; 
    video_init(&video, header, sizeof(header));

    parse_header_status_t status = parse_stream_header(&video);

    CHECK_EQUAL(HDR_ERR_DIM_TOO_LARGE, status);

}


TEST(parseHeaderTests, noColorsInPalette) { 


     const uint8_t header[] =  {
        0x56, 0x43, 
        0x00, 0x80, // Width  = 128
        0x00, 0x80, // Height = 128
        0x00, 0x00, // num_colors = 0, this is not valid
        0xFF
    };

    video_handler_t video; 
    video_init(&video, header, sizeof(header));

    parse_header_status_t status = parse_stream_header(&video);

    CHECK_EQUAL(HDR_ERR_NO_COLORS, status);

}


TEST(parseHeaderTests, tooManyColorsInPalette) { 


     const uint8_t header[] =  {
        0x56, 0x43, 
        0x00, 0x80, // Width  = 128
        0x00, 0x80, // Height = 128
        0x01, 0x01, // num_colors = 257, this is not valid
        0xFF
    };

    video_handler_t video; 
    video_init(&video, header, sizeof(header));

    parse_header_status_t status = parse_stream_header(&video);

    CHECK_EQUAL(HDR_ERR_TOO_MANY_COLORS, status);

}


/*
TEST_GROUP(parsePaletteTests) { 

    void setup() {


    }

    void teardown() { 

    }
};


TEST(parsePaletteTests, successfulParse) { 

    const uint8_t stream[] =  {
        0x56, 0x43, 
        0x00, 0x6C, 
        0x00, 0x7A, 
        0x00, 0x04, 
        0xFF,
         // color 0 
        0xFF, 0x00, 0x00, // red
        // color 1
        0x00, 0xFF, 0x00, // green
        // color 2
        0x00, 0x00, 0xFF, // blue
        // color 3 
        0xFF, 0xFF, 0xFF  // white
    };
    unsigned long video_len = sizeof(stream);



    video_stream_header_t header = {0};
    uint32_t palette[MAX_PALETTE_COLORS]; 


    parse_header_status_t  header_status = parse_stream_header(&header, stream, video_len);    
    parse_palette_status_t palette_status = parse_palette(palette, stream, MAX_PALETTE_COLORS, header.num_colors); 

    CHECK_EQUAL(HDR_OK, header_status);
    CHECK_EQUAL(PAL_OK, palette_status);


    CHECK_EQUAL(true, palette_matches_stream(palette, stream, header.num_colors));


    //printPalette(palette, header.num_colors);

};  


TEST(parsePaletteTests, paletteIncomplete) {

    const uint8_t stream[] = {
        0x56, 0x43,          // magic
        0x00, 0x6C,
        0x00, 0x7A,
        0x01, 0x00,
        0x00,               // num_colors = 0
        0x00,// no palette data
    };

    unsigned long video_len = sizeof(stream);

    video_stream_header_t header = {0};
    uint32_t palette[MAX_PALETTE_COLORS];

    parse_header_status_t header_status = parse_stream_header(&header, stream, video_len);

    parse_palette_status_t palette_status = parse_palette(palette, stream, MAX_PALETTE_COLORS, header.num_colors);

    CHECK_EQUAL(HDR_OK, header_status);
    CHECK_EQUAL(PAL_INCOMPLETE, palette_status);
}
 */