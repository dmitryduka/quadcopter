XTOOLS_LOCATION = ~

AS   = $(XTOOLS_LOCATION)/x-tools/mips-unknown-elf/bin/mips-unknown-elf-as
LD   = $(XTOOLS_LOCATION)/x-tools/mips-unknown-elf/bin/mips-unknown-elf-ld
CC   = $(XTOOLS_LOCATION)/x-tools/mips-unknown-elf/bin/mips-unknown-elf-g++
DUMP = $(XTOOLS_LOCATION)/x-tools/mips-unknown-elf/bin/mips-unknown-elf-objdump

CCOPTS = -O3 -fno-inline -fno-exceptions -fno-rtti
LDOPTS =  -Ttext=0x00000000

all: startup
	$(CC) -c $(CCOPTS) -o main.o main.cc
	$(LD) -o main $(LDOPTS) startup.o main.o
	$(DUMP) -C -Dz --section=.text main > program-objdump.txt
	./objdump_convert program-objdump.txt program.txt

startup:
	$(AS) -mips1 -o startup.o startup.S

functions:
	$(AS) -mips1 -c -o functions.o functions.S

clean:
	rm main program-objdump.txt program.txt *.o