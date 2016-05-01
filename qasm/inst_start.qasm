shl ra10, elem_num, 2  	# ra10 = (0x0000, 0x0004, ... , 0x003c) = TMU0 (0) fetch offsets
mov r0, 64
add ra11, ra10, r0 		# ra11 = (0x0040, 0x0044, ... , 0x007c) = TMU0 (1) fetch offsets
mov r0, 128
add ra12, ra10, r0 		# ra12 = (0x0080, 0x0084, ... , 0x00bc) = TMU0 (2) fetch offsets
add ra13, ra11, r0 		# ra13 = (0x00c0, 0x00c4, ... , 0x00fc) = TMU0 (3) fetch offsets
add ra14, ra12, r0 		# ra14 = (0x0100, 0x00f4, ... , 0x013c) = TMU1 (4) fetch offsets
add ra15, ra13, r0 		# ra15 = (0x0140, 0x0144, ... , 0x017c) = TMU1 (5) fetch offsets
add ra16, ra14, r0 		# ra16 = (0x0180, 0x0184, ... , 0x01bc) = TMU1 (6) fetch offsets
add ra17, ra15, r0 		# ra17 = (0x01c0, 0x01c4, ... , 0x01fc) = TMU1 (7) fetch offsets

mov r0, qpu_num
shl ra18, r0, 9			# ra18 = 0x0000 / 0x0200 / ... / 0x0e00 = QPU TMU fetch offsets
shl ra19, r0, 3			# ra19 = 0x00 / 0x02 / ... / 0x0c = QPU VPM write offsets
