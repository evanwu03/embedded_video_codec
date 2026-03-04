
import cv2
import time 
import numpy as np

# Plays back video on screen
def video_playback(filepath: str) -> None:

    cap = cv2.VideoCapture(filepath)
    fps = cap.get(cv2.CAP_PROP_FPS)
    print(f'FPS: {fps}')
    delay = 1.0/fps

    while cap.isOpened():
        ret, frame = cap.read()
        
        # if frame is read correctly ret is True
        if not ret:
            print("Can't receive frame (stream end?). Exiting ...\n")
            break

        cv2.imshow('frame', frame)
        if cv2.waitKey(1) == ord('q'):
            print('Ending playback...')
            break

        time.sleep(delay)

    cap.release()
    cv2.destroyAllWindows()



def extract_video_frames(filepath: str) -> np.ndarray:
    cap = cv2.VideoCapture(filepath)

    frames = []

    while True:
        ret, current = cap.read()
        if not ret:
            break

        # Vectorized packing: BGR → 0xBBGGRR as uint32
        current = current.astype(np.uint32)
        packed = (
            (current[:,:,0] << 16) | # B
            (current[:,:,1] << 8) |  # G
             current[:,:,2]          # R
        )

        frames.append(packed)

    cap.release()

    # Stack into a single NumPy array: (num_frames, H, W)
    return np.stack(frames, axis=0)