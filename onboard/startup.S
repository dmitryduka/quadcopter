.globl _start

_start:
	li $sp, 0xFFFFFFFF #init stack pointer
	li $gp, 0x00000000 #init global pointer

	jal main #jump to main
	
	sw $v0, 0x00000018 #write main return value to leds

_stop:			#loop forever
	j _stop
