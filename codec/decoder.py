
import numpy as np
import cv2


def rleDecode(stream: bytearray, width: int, height: int, position: int = 0) -> tuple[np.ndarray,  int]:


    out = []
    frame_pixels = width * height
    pixel_count = 0 
    i = position



    while pixel_count < frame_pixels and i < len(stream): 

        # parses the header byte
        run_len = stream[i] & 0x7F 
        is_run = stream[i] & 0x80 != 0

        i += 1 

        # Debug Asserts        
        if run_len == 0: 
            raise ValueError ("Invalid RLE length 0")
        
        if pixel_count + run_len > frame_pixels:
            raise ValueError("RLE token exceeds frame boundary, check encoder implementation")
        
        
        # Following sequence is a consecutive run 
        if is_run:

            # emit the value 
            run_value = stream[i] 

            for k in range(run_len):
               out.append(run_value)

            i += 1 

        else:

            # emit the following literal
            for k in range(run_len):
                out.append(stream[i])
                i += 1
        
        pixel_count += run_len

    return (np.array(out, dtype=np.uint8), i)




# Reads the compressed video binary and decodes the frames and writes them to a video file
def decoder(filename: str, output_file: str, fps: float) -> None: 


    with open(filename, "rb") as f: 
        video = f.read()

    # current position in file 
    pos = 0


    # Parse the stream header 
    format_id = video[pos:pos+2]   
    #print(format_id) 
    if format_id != b"\x56\x43":
        raise ValueError("Invalid video format")
    pos += 2 

    # Get video resolution
    width = int.from_bytes(video[pos:pos+2], "big"); pos += 2
    height = int.from_bytes(video[pos:pos+2], "big"); pos += 2
    
    # Get number of colors in palette
    num_colors = int.from_bytes(video[pos:pos+2], "big")
    pos += 2

    # Retrieves codec flags
    flags = video[pos] # not used yet
    pos += 1


    # Parse palette table
    palette_bytes = video[pos:pos + num_colors * 3] # if palette is 24 bits

    pos += num_colors * 3
    

    palette24 = np.frombuffer(palette_bytes, dtype=np.uint8).reshape(num_colors, 3)

    #print("Decoded palette: ")
    #print(palette24)
    assert palette24.shape == (num_colors, 3)

  
    # Set up VideoWriter
    writer = cv2.VideoWriter(
        output_file,
        cv2.VideoWriter_fourcc(*'mp4v'),
        fps,
        (width, height)
    )

 
    # Decode all compressed frames and write to the output file
    prev_frame = None
    
    with open("output/video.bgr565", "wb") as f:
        while pos < len(video): 

            # RLE decode 
            deltas, pos = rleDecode(video, width, height, pos)

            # Delta decode
            if prev_frame is None:
                curr_idx = deltas
            else:
                curr_idx = (prev_frame + deltas).astype(np.uint8)

            prev_frame = curr_idx

            # Sanity check palette indices aren't out of range
            if np.any(curr_idx < 0) or np.any(curr_idx >= num_colors):
                raise RuntimeError(f"Palette index out of range: " f"min={curr_idx.min()}, max={curr_idx.max()}"
            )

            

            # Palette lookup
            frame24 = palette24[curr_idx].reshape(height, width, 3)
            
            # Convert BGR888 → BGR565
            b = (frame24[:, :, 0] >> 3).astype(np.uint16)
            g = (frame24[:, :, 1] >> 2).astype(np.uint16)
            r = (frame24[:, :, 2] >> 3).astype(np.uint16)

            frame565 = (b << 11) | (g << 5) | r   # uint16 (H, W)
                

            writer.write(frame24)

            f.write(frame565)


    writer.release()

