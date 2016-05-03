.include <helpers.qasm>

sub.setf ra21, ra21, 1		# Decrement the loop counter
mov r0, 4096
add ra18, ra18, r0 			# Update QPU TMU fetch offsets
bra.anynz -, ra20			# Loop condition
nop
nop
nop

END_PROGRAM_HARD
