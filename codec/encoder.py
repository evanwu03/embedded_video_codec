


import numpy as np


# Performs delta compression on a stream of frames and returns 
# a list of frames
def deltaEncode(palette_indices: np.ndarray) -> np.ndarray:


    deltas = []

    prev = None

    for idx_frame in palette_indices: 

        if prev is None:
            deltas.append(idx_frame)
            prev = idx_frame
            continue

        # Compute the delta frame as difference between current and previous frame
        # append it to list and update previous frame value
        #delta_frame = idx_frame.astype(np.int16) - prev.astype(np.int16)
        delta_frame = idx_frame.astype(np.uint8) - prev.astype(np.uint8)


        deltas.append(delta_frame) 

        prev = idx_frame


    return deltas

    
#+----------+--------------------+------------------------------+
#| TOKEN    | BYTE STREAM        | DECODED OUTPUT               |
#+----------+--------------------+------------------------------+
#| RUN      | [1LLLLLLL][VV]     | VV repeated L times          |
#|          |                    |                              |
#| LITERAL  | [0LLLLLLL][V0]...  | V0 V1 ... V(L-1)             |
#|          |            [V(L-1)]|                              |
#+----------+--------------------+------------------------------+

# The table describes the token format for consecutive runs and literals. 
# The RLE uses a 2 byte format where bit 7 of the first byte determines if the following value is 
# a run or literal; bit 0-6 determines how many literals or runs to emit (up to 127) 
def rleEncode(values: np.ndarray) -> np.ndarray:
    
    result = []
    n = len(values)
    i = 0

    RUN_HEADER = 0x80
    LITERAL_HEADER = 0x00


    while i < n:
        # =============================================================
        # Check for encoded run (repetition of same value)
        # =============================================================
        run_val = np.uint8(values[i])
        run_len = 1


        while((i + run_len < n) and (run_len < 127) and (values[i+run_len] == run_val)):
            run_len += 1
    

        if run_len >= 2: 

            header = RUN_HEADER | run_len
            result.append(header)
            result.append(run_val)
            i += run_len
            continue

        # =============================================================
        # Otherwise: LITERAL RUN (collect until next repeated run)
        # =============================================================

        literal_count = 1
        lit_start = i

        while i + literal_count < n and literal_count < 127: 

  
            next_idx = lit_start + literal_count

            if  (next_idx < n - 1) and (int(values[next_idx]) == values[next_idx+1]):
                break

            literal_count += 1
       
        header = LITERAL_HEADER | literal_count 
        result.append(header)

        for j in range(0, literal_count):
            result.append(values[i+j])


        i += literal_count

    return np.array(result, dtype=np.uint8)


""" def rle_vertical_replication(values: np.ndarray) -> np.ndarray:

    result = []
    i = int(0) 
    n = len(values)
    run_len = np.uint8(0)

    while i < n: 

    return 
 """


def compress_video(frames: np.ndarray) -> bytearray:

    compressed_frames = bytearray()

    #Delta Encoding
    delta_frames = deltaEncode(frames)

    # Dumps delta frames in a txt file
    """ with open(FRAME_TXT_DUMP, "w") as f:
        for i, frame in enumerate(delta_frames):
            f.write(f"# --- Frame {i} ---\n")
            np.savetxt(f, frame, fmt="%x")
            f.write("\n\n") """


    # ==============================
    # Byte Level RLE encoding scheme
    # ==============================

    # Before encoding
    # total_frames = len(delta_frames)
    # print(f'Total number of frames: {total_frames}')

    # Perform -> RLE 
    for frame in delta_frames: 
        
        rle_frame = rleEncode(frame.ravel())

        compressed_frames.extend(rle_frame)

    # Debugging information
    print(f'Total number of bytes after RLE: {len(compressed_frames)}')

    return compressed_frames