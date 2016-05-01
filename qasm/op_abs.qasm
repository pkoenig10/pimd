# From https://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs

asr rb0, ra0, 31
add ra0, ra0, rb0
xor ra0, ra0, rb0

asr rb1, ra1, 31
add ra1, ra1, rb1
xor ra1, ra1, rb1

asr rb2, ra2, 31
add ra2, ra2, rb2
xor ra2, ra2, rb2

asr rb3, ra3, 31
add ra3, ra3, rb3
xor ra3, ra3, rb3

asr rb4, ra4, 31
add ra4, ra4, rb4
xor ra4, ra4, rb4

asr rb5, ra5, 31
add ra5, ra5, rb5
xor ra5, ra5, rb5

asr rb6, ra6, 31
add ra6, ra6, rb6
xor ra6, ra6, rb6

asr rb7, ra7, 31
add ra7, ra7, rb7
xor ra7, ra7, rb7
