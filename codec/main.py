
import numpy as np
import time 

from quantizer import generate_palette
from quantizer import quantize_pixels


from encoder import compress_video

from decoder import decoder


from video import video_playback
from video import extract_video_frames

from config import *


def main(): 

    start_time = time.time()

    # Optional: play back video you plan to compress
    video_playback(FILEPATH)


    video = extract_video_frames(FILEPATH)
    print(f'Video Resolution: {video.shape}')

    # Video Resolution specs
    height = video.shape[1]
    width = video.shape[2]
    num_frames = len(video)

    # Flatten to 1D list of pixels
    pixels = np.concatenate([frame.ravel() for frame in video])

    uncompressed_bytes = len(pixels)*4
    print(f'Uncompressed file size: {uncompressed_bytes}')

    # Generate Color palette
    NUM_COLOR = 256
    color_palette = generate_palette(pixels, NUM_COLOR)

    # Stack BGR channels into 3 separate byte vectors 
    palette_b = (color_palette >> 16)  & 0xFF
    palette_g = (color_palette >> 8)  & 0xFF
    palette_r = (color_palette >> 0) & 0xFF
    palette_bgr = np.stack([palette_b, palette_g, palette_r], axis=1).astype(np.uint8)    
    assert palette_bgr.shape == (NUM_COLOR, 3)

    # Quantize frames 
    quantization_start_time = time.time()

    # Preallocate array and quantize frame by frame instead still kind of a brute force but no longer trying to compute all pairwise distances at once
    # and devouring memory
    quantized_frames = np.empty((num_frames, height, width), dtype=np.uint8)
    for i in range(num_frames):

        frame_pixels = video[i].reshape(-1)   
        q = quantize_pixels(frame_pixels, color_palette)
        quantized_frames[i] = q.reshape(height, width)

    assert quantized_frames.min() >= 0 
    assert quantized_frames.max() < NUM_COLOR
    quantization_finish_time = time.time()
    print(f'Total quantization time: {(quantization_finish_time-quantization_start_time):.2f}')

   
    # append global header to byte array. format of final C stream will be as follows 
    # [HEADER] 
    # Byte 0-1 : Magic            (e.g. 0x56 0x43 = "VC")
    # Byte 2-3   : Width            (e.g. 128 pixels)
    # Byte 4-5   : Height           (e.g. 128 pixels )
    # Byte 6-7   : Num colors       (palette size: e.g. 256)
    # Byte 8   : Flags            (delta, zigzag, etc.)
    # [PALETTE] 
    # [FRAME STREAM]

    # Compress video
    encoded_frames = bytearray() 
    encoded_frames = compress_video(quantized_frames)

    compressed_bytes = len(encoded_frames)
    print(f'Total Compression Ratio: {uncompressed_bytes/compressed_bytes:.2f}')
    
    # Write compressed video to binary file
    with open(ENCODED_BIN, "wb") as f:
        f.write(b"\x56\x43")                        # decoder expects this to identify video format
        f.write(width.to_bytes(2, "big"))           # Width of frames
        f.write(height.to_bytes(2, "big"))          # Height of frames
        f.write((NUM_COLOR).to_bytes(2, "big"))     # number of colors in palette
        f.write(bytes([0xFF]))                      # Dummy byte for flags to be defined later
        f.write(palette_bgr.tobytes())                  
        f.write(encoded_frames)                    


    with open(ENCODED_BIN, "rb") as f:
        data = f.read()


    # Generate C style array to be used by an MCU
    with open("../src/video_data.inc", "w") as f:
        for i, b in enumerate(data):
            if i % 12 == 0:
                f.write("\n")
            f.write(f"0x{b:02X}, ")


    # Optional: Decoder video for playback 
    decoder(ENCODED_BIN, "output/video_decoded.mp4", fps=25.0)


    end_time = time.time()
    print(f'Total time elapsed: {end_time-start_time:.2f}')

    # Playback compressed video
    video_playback("output/video_decoded.mp4")

if __name__ == "__main__":
    main()