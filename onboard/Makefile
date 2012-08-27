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
CCOPTS = -G0 -nostartfiles -I. -c -fno-threadsafe-statics -fpack-struct -fstack-usage -Wstack-usage=$(STACK_SIZE) -std=c++11 -Os -fno-strict-aliasing -fno-exceptions -fno-rtti

SRCDIR = src
OBJDIR = obj
LISTINGSDIR = listings

PROGRAM = main
IMEMFILE = program.txt
DMEMFILE = data.txt

SRC_AS = $(wildcard $(SRCDIR)/*.S)
SRC_CC = $(wildcard $(SRCDIR)/*.cc) $(wildcard $(SRCDIR)/*.cpp)
SRC_C = $(wildcard $(SRCDIR)/*.c)

OBJECTS = $(foreach file, $(SRC_AS), $(OBJDIR)/$(notdir $(file)).o)
OBJECTS += $(foreach file, $(SRC_CC), $(OBJDIR)/$(notdir $(file)).o)
OBJECTS += $(foreach file, $(SRC_C), $(OBJDIR)/$(notdir $(file)).o)

DEPS = $(wildcard $(OBJDIR)/*.d)

all: dirs $(IMEMFILE) $(DMEMFILE)

include $(DEPS)

main: $(OBJECTS)
	$(LD) -T $(SRCDIR)/ld.script -o main $(LDOPTS) $(OBJECTS)
	$(READELF) -a main > $(LISTINGSDIR)/program-elf.txt

$(IMEMFILE): main
	$(DUMP) -C -Dz --section=.text main > $(LISTINGSDIR)/program-listing.txt
	cat $(LISTINGSDIR)/program-listing.txt | grep -P "^\s*[\d|abcdef]+:" | cut -f 2 > program.txt

$(DMEMFILE): main
	-$(DUMP) -C -Dz --section=.data main > $(LISTINGSDIR)/data-listing.txt
	-cat $(LISTINGSDIR)/data-listing.txt | grep -P "^\s*[\d|abcdef]+:" | cut -f 2 > data.txt

$(OBJDIR)/%.cc.o: $(SRCDIR)/%.cc
	$(CC) $(CCOPTS) $< -o $@
	$(CC) -MM -MT $(OBJDIR)/$*.cc.o $< -o $(OBJDIR)/$*.d

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	$(CC) $(CCOPTS) $< -o $@
	$(CC) -MM -MT $(OBJDIR)/$*.c.o $< -o $(OBJDIR)/$*.d

$(OBJDIR)/%.S.o: $(SRCDIR)/%.S
	$(AS) -mips1 $< -o $@

dirs: $(OBJDIR) $(LISTINGSDIR)

$(OBJDIR) $(LISTINGSDIR):
	mkdir -p $@

clean:
	-rm -rf $(OBJDIR)
	-rm -rf $(LISTINGSDIR)
	-rm main program.txt data.txt