.globl _start

_start:
	li $sp, 0xFFFFFFFF	# init stack pointer
	lw $v0, CTORS_INIT	# load ctors init function
	jalr $v0		# call ctors
	jal main		# jump to main
