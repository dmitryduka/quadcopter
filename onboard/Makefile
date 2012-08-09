XTOOLS_LOCATION = ~/x-tools-new
XTOOLS_TRIPLE = mips-quadcopter-elf

AS   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-as
LD   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-ld
CC   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-g++
DUMP = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-objdump
READELF = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-readelf
STRIP = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-strip

CCOPTS = -std=c++11 -O3 -fno-inline -fno-exceptions -fno-rtti
LDOPTS = -Ttext=0x00000000

all: startup
	$(CC) -c $(CCOPTS) -o memmgr.o memmgr.cc
	$(CC) -c $(CCOPTS) -o main.o main.cc
	$(LD) -o main $(LDOPTS) startup.o main.o memmgr.o
	$(DUMP) -C -Dz --section=.text main > program-objdump.txt
	$(READELF) -a main > program-elf.txt
	./objdump_convert program-objdump.txt program.txt

startup:
	$(AS) -mips1 -o startup.o startup.S

functions:
	$(AS) -mips1 -c -o functions.o functions.S

clean:
	rm main program-objdump.txt program.txt *.o