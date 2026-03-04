

import cv2
import numpy as np
from quantizer import generate_palette, quantize_pixels

# -----------------------------
# Load image
# -----------------------------
img = cv2.imread("videos/apple.jpeg")   # BGR888
if img is None:
    raise RuntimeError("Failed to load image")

h, w, _ = img.shape

# -----------------------------
# Flatten pixels
# -----------------------------
# Vectorized packing: BGR → 0xBBGGRR as uint32

pixels = img.astype(np.uint32)
pixels = (    
    (pixels[:,:,0] << 16) | # B
    (pixels[:,:,1] << 8) |  # G
     pixels[:,:,2]          # R
)

pixels = pixels.ravel()

print(pixels)
print(pixels.shape)
# -----------------------------
# Generate palette
# -----------------------------
NUM_COLORS = 64
assert NUM_COLORS <= 256

palette = generate_palette(pixels, NUM_COLORS)   # expected shape (256, 3)

# -----------------------------
# Quantize
# -----------------------------
indices = quantize_pixels(pixels, palette)

assert indices.min() >= 0
assert indices.max() < NUM_COLORS

# -----------------------------
# Reconstruct image
# -----------------------------
quantized_pixels = palette[indices].reshape(h,w)              # fancy indexing
quantized_img = np.empty((h, w, 3), dtype = np.uint8) 
quantized_img[:, :, 0] = (quantized_pixels >> 16) & 0xFF  # B
quantized_img[:, :, 1] = (quantized_pixels >> 8) & 0xFF   # G
quantized_img[:, :, 2] = quantized_pixels & 0xFF          # R


# -----------------------------
# Display
# -----------------------------
cv2.imshow("Original", img)
cv2.imshow("Quantized", quantized_img)
cv2.waitKey(0)
cv2.destroyAllWindows()
