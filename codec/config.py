
import os

# Filepaths
#FILENAME = 'ordinary.mp4'
#FILENAME = 'ordinary_12fps.mp4'
#FILENAME = 'ordinary_10fps.mp4'
#FILENAME = 'ordinary_96x96_12fps.mp4'
#FILENAME = 'ordinary_96x96_10fps.mp4'
#FILENAME = 'kanade_128x128.mp4'
#FILENAME = 'kanade_128x128_12fps.mp4'
#FILENAME  = 'kanade_12fps.mp4'
#FILENAME  = 'kanade_10fps.mp4'
#FILENAME = 'ryo_yamada_128x128.mp4'
#FILENAME = 'ryo_yamada_128x128_12fps.mp4'
#FILENAME  = 'ryo2.mp4'
#FILENAME = 'kikuri.mp4'
#FILENAME = 'kikuri_10fps.mp4'
#FILENAME = 'kikuri_20fps.mp4'
#FILENAME = 'ragebaited.mp4'
#FILENAME = 'bocchi.mp4'
#FILENAME = 'bocchi-dance4.mp4'
#FILENAME = 'idk_10fps.mp4'
#FILENAME = 'kaela.mp4'
#FILENAME = 'kaela_20fps.mp4'
#FILENAME = 'kaela_12fps.mp4'
#FILENAME  = 'raden.mp4'
#FILENAME  = 'raden_20fps.mp4'
#FILENAME  = 'raden_12fps.mp4'
#FILENAME = 'trampoline3.mp4'
#FILENAME = 'btr.mp4'
#FILENAME = 'btr2.mp4'
#FILENAME = 'btr3.mp4'
#FILENAME = 'furry.mp4'
#FILENAME = 'bad_apple2.mp4'
FILENAME = 'uma_128.mp4'
#FILENAME = 'losercity_rigby_128.mp4'
#FILENAME = 'perlica_30k_128.mp4'

BASE = os.path.dirname(os.path.abspath(__file__))
FILEPATH = os.path.join(BASE, 'videos', FILENAME)
FRAME_TXT_DUMP = os.path.join(BASE, 'output', 'delta.txt')
DELTA_BIN = os.path.join(BASE, 'output', 'delta.bin')
ENCODED_TXT_DUMP = os.path.join(BASE, 'output', 'encoded.txt')
ENCODED_BIN  = os.path.join(BASE, 'output', 'encoded.bin')
DECODED_TXT_DUMP = os.path.join(BASE, 'output', 'decoded_pixels.txt')
DECODED_BIN = os.path.join(BASE, 'output', 'decoded_pixels.bin')

