# Implements 32 bit multiply using binomial multiplication

mov r2, 24

asr r0, ra0, r2
asr r1, rb0, r2
mul24 r0, r0, rb0
mul24 r1, ra0, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, ra0, rb0
add r3, r3, r0
add ra0, r3, r1

asr r0, ra1, r2
asr r1, rb1, r2
mul24 r0, r0, rb1
mul24 r1, ra1, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, ra1, rb1
add r3, r3, r0
add ra1, r3, r1

asr r0, ra2, r2
asr r1, rb2, r2
mul24 r0, r0, rb2
mul24 r1, ra2, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, ra2, rb2
add r3, r3, r0
add ra2, r3, r1

asr r0, ra3, r2
asr r1, rb3, r2
mul24 r0, r0, rb3
mul24 r1, ra3, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, ra3, rb3
add r3, r3, r0
add ra3, r3, r1

asr r0, ra4, r2
asr r1, rb4, r2
mul24 r0, r0, rb4
mul24 r1, ra4, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, ra4, rb4
add r3, r3, r0
add ra4, r3, r1

asr r0, ra5, r2
asr r1, rb5, r2
mul24 r0, r0, rb5
mul24 r1, ra5, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, ra5, rb5
add r3, r3, r0
add ra5, r3, r1

asr r0, ra6, r2
asr r1, rb6, r2
mul24 r0, r0, rb6
mul24 r1, ra6, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, ra6, rb6
add r3, r3, r0
add ra6, r3, r1

asr r0, ra7, r2
asr r1, rb7, r2
mul24 r0, r0, rb7
mul24 r1, ra7, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, ra7, rb7
add r3, r3, r0
add ra7, r3, r1
