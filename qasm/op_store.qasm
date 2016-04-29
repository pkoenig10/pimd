.include <helpers.qasm>

.global master
.global slave
.global end

mov.setf -, qpu_num
mov r2, VPM_WRITE_SETUP_VALUE(1, IS_HORIZ, NOT_LANED, SIZE_32_BIT, 0)
brr.allz -, r:master
or vw_setup, r2, rb2
mov vpm, r0
mov vpm, r1

slave:
	brr -, r:end
	srel -, 0		# Signal to master
	sacq -, 1		# Wait for master
	nop

master:
	# Wait for slaves
	sacq -, 0
	sacq -, 0
	sacq -, 0
	sacq -, 0
	sacq -, 0
	sacq -, 0
	sacq -, 0

	VPM_STORE_SETUP 64, 16, IS_HORIZ, 0, 0, MODEW_32_BIT
	VPM_STORE_START unif
	VPM_STORE_WAIT
	
	# Signal to slaves
	srel -, 1
	srel -, 1
	srel -, 1
	srel -, 1
	srel -, 1
	srel -, 1
	srel -, 1

end: