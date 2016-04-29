# From https://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs

asr r2, r0, 31
add r0, r0, r2
xor r0, r0, r2

asr r3, r1, 31
add r1, r1, r3
xor r1, r1, r3
