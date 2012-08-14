XTOOLS_LOCATION = ~/x-tools-new
XTOOLS_TRIPLE = mips-quadcopter-elf

AS   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-as
LD   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-ld
CC   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-g++
DUMP = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-objdump
READELF = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-readelf
STRIP = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-strip

# Optimizing for speed is actually optimizing for size in our case
CCOPTS = -std=c++11 -Os -fno-strict-aliasing -fno-exceptions -fno-rtti
LDOPTS = --no-check-sections --section-start .text=0x00000000 --section-start .rodata=0x00000000 -n -N

all: startup
	$(CC) -c $(CCOPTS) -o memmgr.o memmgr.cc
	$(CC) -c $(CCOPTS) -o main.o main.cc
	$(LD) -o main $(LDOPTS) startup.o main.o memmgr.o
	$(DUMP) -C -Dz --section=.text main > program-objdump.txt
	$(DUMP) -C -Dz --section=.rodata main > program-data.txt
	$(READELF) -a main > program-elf.txt
	./objdump_convert program-objdump.txt program.txt
	./objdump_convert program-data.txt data.txt

startup:
	$(AS) -mips1 -o startup.o startup.S

functions:
	$(AS) -mips1 -c -o functions.o functions.S

clean:
	rm main program-objdump.txt program.txt *.o