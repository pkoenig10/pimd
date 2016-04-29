# Implements 32 bit multiply using binomial multiplication

asr ra0, r0, 24
asr ra2, r2, 24
mul24 ra0, ra0, r2
mul24 ra2, r0, ra2
shl ra0, ra0, 24
shl ra2, ra2, 24
mul24 r0, r0, r2
add r0, r0, ra0
add r0, r0, ra2

asr ra1, r1, 24
asr ra3, r3, 24
mul24 ra1, ra1, r3
mul24 ra3, r1, ra3
shl ra1, ra1, 24
shl ra3, ra3, 24
mul24 r0, r0, r2
add r0, r0, ra1
add r0, r0, ra3
