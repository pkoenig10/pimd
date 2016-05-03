# Implements 32 bit multiply using binomial multiplication

.include "pimd.qinc"

mov r2, 24

asr r0, var0, r2
asr r1, arg0, r2
mul24 r0, r0, arg0
mul24 r1, var0, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, var0, arg0
add r3, r3, r0
add var0, r3, r1

asr r0, var1, r2
asr r1, arg1, r2
mul24 r0, r0, arg1
mul24 r1, var1, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, var1, arg1
add r3, r3, r0
add var1, r3, r1

asr r0, var2, r2
asr r1, arg2, r2
mul24 r0, r0, arg2
mul24 r1, var2, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, var2, arg2
add r3, r3, r0
add var2, r3, r1

asr r0, var3, r2
asr r1, arg3, r2
mul24 r0, r0, arg3
mul24 r1, var3, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, var3, arg3
add r3, r3, r0
add var3, r3, r1

asr r0, var4, r2
asr r1, arg4, r2
mul24 r0, r0, arg4
mul24 r1, var4, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, var4, arg4
add r3, r3, r0
add var4, r3, r1

asr r0, var5, r2
asr r1, arg5, r2
mul24 r0, r0, arg5
mul24 r1, var5, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, var5, arg5
add r3, r3, r0
add var5, r3, r1

asr r0, var6, r2
asr r1, arg6, r2
mul24 r0, r0, arg6
mul24 r1, var6, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, var6, arg6
add r3, r3, r0
add var6, r3, r1

asr r0, var7, r2
asr r1, arg7, r2
mul24 r0, r0, arg7
mul24 r1, var7, r1
shl r0, r0, r2
shl r1, r1, r2
mul24 r3, var7, arg7
add r3, r3, r0
add var7, r3, r1
