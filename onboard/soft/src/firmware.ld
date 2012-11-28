/* Default linker script, for normal executables */
OUTPUT_FORMAT("elf32-bigmips", "elf32-bigmips",
	      "elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(_start)
SEARCH_DIR(".")
MEMORY 
{
    imem (rwx) : ORIGIN = 0, LENGTH = 32K
    dmem (rw)  : ORIGIN = 0x80000000, LENGTH = 4K
}

NOCROSSREFS ( .boot .text )

SECTIONS
{
  /DISCARD/ : {
    *(.jcr)
    *(.jcr.*)
    *(.eh_frame)
    *(.eh_frame.*)
  }

  .init 0: {
    *(.codeinit)
  } > imem

  CODEINIT_ENDS = .;

  BOOTLOADER_STARTS = .;
  .boot CODEINIT_ENDS : {
    *(.bootloader)
  } > imem

  BOOTLOADER_MAIN_ENDS = .;

  .tramp BOOTLOADER_MAIN_ENDS : {
    *(.trampoline)
  } > imem

  BOOTLOADER_ENDS = .;
  BOOTLOADER_SIZE = SIZEOF(.boot) + SIZEOF(.tramp);

  .text BOOTLOADER_ENDS : {
    CODE_STARTS = .;
    *(.text.startup)
    *(.text)
    CODE_ENDS = .;
    . = LENGTH(imem) - BOOTLOADER_ENDS;
  } > imem = 0

  .data : {
    *(.rodata .rodata.* .gnu.linkonce.r.*)
    *(.data .data.* .gnu.linkonce.d.*)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
    *(.dynsbss)
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
    *(.dynbss)
    *(.bss .bss.* .gnu.linkonce.b.*)
    *(COMMON)
    CTORS_INIT = .;
    KEEP(*(.ctors))
    KEEP(*(.ctors.*))
    *(.dtors)
    *(.dtors.*)
    DATA_ENDS = .;
    . = LENGTH(dmem);
  } > dmem = 0
}
