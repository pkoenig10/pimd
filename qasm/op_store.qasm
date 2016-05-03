.include <helpers.qasm>

mov.setf -, qpu_num
mov r0, VPM_WRITE_SETUP_VALUE(1, IS_HORIZ, NOT_LANED, SIZE_32_BIT, 0)
or vw_setup, r0, ra19
MUTEX_ACQUIRE
mov vpm, ra0
mov vpm, ra1
mov vpm, ra2
mov vpm, ra3
mov vpm, ra4
mov vpm, ra5
brr.allz -, r:master
mov vpm, ra6
mov vpm, ra7
MUTEX_RELEASE

# mov r0, VPM_STORE_SETUP_VALUE(8, 16, IS_HORIZ, 0, 0, MODEW_32_BIT)
# shl r1, ra19, VPM_STORE_SETUP_ADDRY_SHIFT
# or vw_setup, r0, r1
# VPM_STORE_START unif, ra18
# VPM_STORE_WAIT

slave:
	brr -, r:end
	srel -, 0		# Signal to master
	sacq -, 1		# Wait for master
	read unif		# Discard store address

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
	VPM_STORE_START unif, ra18
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
