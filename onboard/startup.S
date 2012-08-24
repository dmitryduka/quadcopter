.globl _start

_start:
	li $sp, 0xFFFFFFFF #init stack pointer
	jal main #jump to main
