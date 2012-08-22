XTOOLS_LOCATION = ~/x-tools-new
XTOOLS_TRIPLE = mips-quadcopter-elf

AS   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-as
LD   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-ld
CC   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-g++
DUMP = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-objdump
READELF = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-readelf
STRIP = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-strip

# Optimizing for speed is actually optimizing for size in our case
STACK_SIZE = 2000
CCOPTS = -I. -c -fstack-usage -Wstack-usage=$(STACK_SIZE) -std=c++11 -Os -fno-strict-aliasing -fno-exceptions -fno-rtti
LDOPTS = --no-check-sections --section-start .text=0x00000000 --section-start .rodata=0x00000000 -n -N

CCSOURCES = main.cc memmgr.cc
OBJECTS = $(CCSOURCES:.cc=.o)
OBJECTS += $(CSOURCES:.c=.o)
OBJECTS += startup.o


all: $(OBJECTS) startup.o
	$(LD) -o main $(LDOPTS) $(OBJECTS)
	$(DUMP) -C -Dz --section=.text main > program-listing.txt
	$(DUMP) -C -Dz --section=.rodata main > data-listing.txt
	cat program-listing.txt | grep -P "^\s*[\d|abcdef]+:" | cut -f 2 > program.txt
	cat data-listing.txt | grep -P "^\s*[\d|abcdef]+:" | cut -f 2 > data.txt
	$(READELF) -a main > program-elf.txt

startup.o:
	$(AS) -mips1 -o startup.o startup.S

.cc.o:
	$(CC) $(CCOPTS) $< -o $@

.c.o:
	$(CC) $(CCOPTS) $< -o $@

functions:
	$(AS) -mips1 -c -o functions.o functions.S

clean:
	rm main program-objdump.txt program.txt *.o