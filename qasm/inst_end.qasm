.include "pimd.qinc"

mov r0, 4096
add vdw_offset, vdw_offset, r0       # Update QPU TMU fetch offsets

loop_end main_loop, main_count

mov.setf -, qpu_num
brr.allnz -, r:slave
nop
nop
nop

master:
	exit 1

slave:
	exit 0
