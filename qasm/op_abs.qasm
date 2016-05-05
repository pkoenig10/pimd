# From https://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs

.include "pimd.qinc"

.foreach val, val0, val1, val2, val3, val4, val5, val6, val7
  asr r0, val, 31
  add r1, val, r0
  xor val, r0, r1
.endr
