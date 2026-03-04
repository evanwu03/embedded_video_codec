
import numpy as np
import time 

from quantizer import generate_palette
from quantizer import quantize_pixels


from encoder import compress_video

from decoder import decoder


from video import video_playback
from video import extract_video_frames

from config import *
from pathlib import Path
import argparse



from pathlib import Path
import sys


def validate_input(path_str):
    p = Path(path_str)

    if not p.exists():
        raise FileNotFoundError(f"{path_str} does not exist")

    if not p.is_file():
        raise ValueError(f"{path_str} is not a file")

    return p


def main(): 

    start_time = time.time()


    # Parse Arguments 
    parser = argparse.ArgumentParser()
    parser.add_argument("input", help="Input video file path (mp4/mov/gif/etc)", type=str)
    parser.add_argument("--colors", help="Number of colors in palette", default=256, type=int)
    parser.add_argument("--preview", default=True, help="Optional preview output (gif/mp4)")

    args = parser.parse_args()


    # Get path of video file

    try: 

        filepath = validate_input(args.input)

        num_colors = args.colors


        # DEBUG: don't let users 
        assert args.colors <= 256

    
        # Optional: play back video you plan to compress
        if args.preview:
            print("==============================")
            print("Playing preview of video...")
            video_playback(filepath)
            print("Finished playing video")
            print("==============================")



        print("Extracting frames from video... ")
        video = extract_video_frames(filepath)
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
        color_palette = generate_palette(pixels, num_colors)

        # Stack BGR channels into 3 separate byte vectors 
        palette_b = (color_palette >> 16)  & 0xFF
        palette_g = (color_palette >> 8)  & 0xFF
        palette_r = (color_palette >> 0) & 0xFF
        palette_bgr = np.stack([palette_b, palette_g, palette_r], axis=1).astype(np.uint8)    
        assert palette_bgr.shape == (num_colors, 3)

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
        assert quantized_frames.max() < num_colors
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
        encoded_frames = compress_video(quantized_frames)
        print("==============================")
        print("Completed video compression. Writing output to disk")

        compressed_bytes = len(encoded_frames)
        print(f'Total Compression Ratio: {uncompressed_bytes/compressed_bytes:.2f}')
        
        # Write compressed video to binary file
        with open(ENCODED_BIN, "wb") as f:
            f.write(b"\x56\x43")                        # decoder expects this to identify video format
            f.write(width.to_bytes(2, "big"))           # Width of frames
            f.write(height.to_bytes(2, "big"))          # Height of frames
            f.write((num_colors).to_bytes(2, "big"))     # number of colors in palette
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
        if args.preview:
            print("==============================")
            print("Playing back compressed video")
            video_playback("output/video_decoded.mp4")
    

    except Exception as e:
        print(f"[Runtime Error]: {e}")


if __name__ == "__main__":
    main()