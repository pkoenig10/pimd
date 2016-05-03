# From https://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs

.include "pimd.qinc"

.foreach var, var0, var1, var2, var3, var4, var5, var6, var7
  asr r0, var, 31
  add r1, var, r0
  xor var, r0, r1
.endr
