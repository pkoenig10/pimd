# Implements 32 bit multiply using binomial multiplication

.include "pimd.qinc"

mov r2, 24

asr r0, val0, r2
asr r1, arg0, r2
mul24 r0, r0, arg0
mul24 r1, val0, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, val0, arg0
add r3, r3, r0
add val0, r3, r1

asr r0, val1, r2
asr r1, arg1, r2
mul24 r0, r0, arg1
mul24 r1, val1, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, val1, arg1
add r3, r3, r0
add val1, r3, r1

asr r0, val2, r2
asr r1, arg2, r2
mul24 r0, r0, arg2
mul24 r1, val2, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, val2, arg2
add r3, r3, r0
add val2, r3, r1

asr r0, val3, r2
asr r1, arg3, r2
mul24 r0, r0, arg3
mul24 r1, val3, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, val3, arg3
add r3, r3, r0
add val3, r3, r1

asr r0, val4, r2
asr r1, arg4, r2
mul24 r0, r0, arg4
mul24 r1, val4, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, val4, arg4
add r3, r3, r0
add val4, r3, r1

asr r0, val5, r2
asr r1, arg5, r2
mul24 r0, r0, arg5
mul24 r1, val5, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, val5, arg5
add r3, r3, r0
add val5, r3, r1

asr r0, val6, r2
asr r1, arg6, r2
mul24 r0, r0, arg6
mul24 r1, val6, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, val6, arg6
add r3, r3, r0
add val6, r3, r1

asr r0, val7, r2
asr r1, arg7, r2
mul24 r0, r0, arg7
mul24 r1, val7, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, val7, arg7
add r3, r3, r0
add val7, r3, r1
