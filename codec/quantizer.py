

import numpy as np
import time 



def generate_palette(bucket: np.ndarray, num_colors=256)  -> np.ndarray:
 
    target_depth = int(np.log2(num_colors)) 
    assert 2**target_depth == num_colors # "Num colors must be a power of 2!"
    palette = median_cut_recurse(bucket, 0, target_depth)
    return np.array(palette)



def median_cut_recurse(bucket: np.ndarray, level: int, target_depth: int) -> np.ndarray:

    if level == target_depth:
        return [averageColor(bucket)]
    
    lower_palette, upper_palette = split_bucket(bucket)

    left_colors = median_cut_recurse(lower_palette, level+1, target_depth)
    right_colors = median_cut_recurse(upper_palette, level+1, target_depth)

    return left_colors + right_colors



def split_bucket(pixels: np.ndarray) -> np.ndarray: 

    key = None

    B = (pixels >> 16) & 0xFF
    G = (pixels >> 8)  & 0xFF
    R = pixels         & 0xff

    b_range = B.max() - B.min()
    g_range = G.max() - G.min()
    r_range = R.max() - R.min()

    if b_range >= g_range and b_range >= r_range:
        key = (pixels >> 16) & 0xFF     # blue
    elif g_range >= r_range:
        key = (pixels >> 8) & 0xFF      # green
    else:
        key = pixels & 0xFF             # red

    # NumPy sort using a key
    pixels = pixels[np.argsort(key, kind='stable')]

    lower, upper = np.array_split(pixels, 2)

    """ print("\n--- DEBUG SORT CHECK ---")
    print("Showing first 20 sorted pixels AND extracted channels:")

    for px in pixels[:20]:
        B = (px >> 16) & 0xFF
        G = (px >> 8)  & 0xFF
        R =  px        & 0xFF
        print(f"{px:06X}   B={B:3}  G={G:3}  R={R:3}")
    print("-------------------------\n") """ 
    #print(f'len(lower): {len(lower)}')
    #print(f'len(upper): {len(upper)}')

    return (lower, upper)


def averageColor(pixels: np.ndarray) -> np.ndarray:
     
     if len(pixels) == 0:
         print("Did not find any pixels to average")
         return 0
     
     B = int(((pixels >> 16) & 0xFF).mean())
     G = int(((pixels >> 8)  & 0xFF).mean())
     R = int(( pixels        & 0xFF).mean())

     return (B << 16) | (G << 8) | R


# To-Do Brute force calculating the euclidean distance is too slow, perhaps we use a K-d tree instead
def quantize_pixels(pixels: np.ndarray, palette: np.ndarray) -> np.ndarray:
    
    
    #print("pixels contain NaN:", np.isnan(pixels).any())
    #print("palette contain NaN:", np.isnan(palette).any())


    pR, pG, pB = unpack_rgb(pixels)
    cR, cG, cB = unpack_rgb(palette)


    pB = pB.astype(np.float32)
    pG = pG.astype(np.float32)
    pR = pR.astype(np.float32)

    cB = cB.astype(np.float32)
    cG = cG.astype(np.float32)
    cR = cR.astype(np.float32)
   

    P = np.stack([pR, pG, pB], axis=1).astype(np.float32)
    C = np.stack([cR, cG, cB], axis=1).astype(np.float32) 

    # Precompute norms
    P2 = np.sum(P*P, axis=1)[:, None] # (N,1)
    C2 = np.sum(C*C, axis=1)[None, :] # (1, K)

    start = time.time()
    dist2 = P2 + C2 - 2.0 * (P @ C.T)
    end = time.time()

    if np.any(np.isnan(dist2)):
        raise ValueError("NaNs detected in distance matrix")
    
    indices = np.argmin(dist2, axis=1).astype(np.uint8) # uint8 because we don't expect k > 256

    #print(f'Time to compute euclidean distances: {end-start:.2f}')

    return  indices


def unpack_rgb(packed): # Needs type hint
    R = (packed & 0xFF).astype(np.uint16)
    G = ((packed >> 8) & 0xFF).astype(np.uint16)
    B = ((packed >> 16) & 0xFF).astype(np.uint16)
    return R, G, B
