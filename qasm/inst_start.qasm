mov r0, 2
shl rb0, elem_num, r0  	# rb0 = (0x00, 0x04, ... , 0x3c) = TMU0 fetch offsets
mov r0, 64
add rb1, rb0, r0 		# rb1 = (0x40, 0x40, ... , 0x7c) = TMU1 fetch offsets

mov r0, 1
shl rb2, qpu_num, r0	# rb2 = (0x00, 0x02, ... , 0x0c) = VPM write offsets
