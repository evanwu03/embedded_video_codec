
import numpy as np
from encoder import rleEncode
from encoder import zigzagEncode
from decoder import *

test_data = np.zeros(1000, dtype=np.uint16)      
out = rleEncode(test_data)

print(f'Test1:')
for v in out:
    #print(hex(v), end=" ")
    print(v, end=" ")

print('\n')

test_data = np.array([0,0,0,5,6,0,0,0], dtype=np.uint16)
out = rleEncode(test_data)

print(f'Test2:')
for v in out:
    print(v, end=" ")

print('\n')


test_data = np.array([0,0,0,128,6,128,0], dtype=np.uint16)
out = rleEncode(test_data)

print(f'Test3:')
for v in out:
    print(v, end=" ")

print('\n')


print(f'Zigzag test:')

x = np.random.randint(-127, 127, size=1000, dtype=np.int16)
assert np.all(zigzagDecode(zigzagEncode(x)) == x)
print("Passed test")

