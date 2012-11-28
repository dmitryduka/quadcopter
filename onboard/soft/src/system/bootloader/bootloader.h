#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#define NORETURN __attribute__ ((noreturn))
#define BOOTLOADER_NORETURN __attribute__ ((noreturn, section(".bootloader")))

extern "C" void _start() NORETURN;
extern "C" void bootloader_enter();
extern "C" void bootloader_main() BOOTLOADER_NORETURN;

#endif