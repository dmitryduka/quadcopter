.section .trampoline
.globl bootloader_enter

bootloader_enter:
	li $sp, 0xFFFFFFFF	# init stack pointer
	jal bootloader_main		# jump to the bootloader
