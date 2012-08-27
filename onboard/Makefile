XTOOLS_LOCATION = ~/x-tools-new
XTOOLS_TRIPLE = mips-quadcopter-elf

AS   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-as
LD   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-ld
CC   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-g++
DUMP = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-objdump
READELF = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-readelf
STRIP = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-strip

GRN = tput setaf 2; echo $(1); tput sgr0
YLW = tput setaf 3; echo $(1); tput sgr0
RED = tput setaf 1; echo $(1); tput sgr0
BLD = tput bold; echo -n $(1); tput sgr0
CHECK = if test -e temp.errors; then $(call RED, $(1)) && cat temp.log; elif test -s temp.log; then $(call YLW, $(1)) && cat temp.log; else $(call GRN, '$(1) no warnings/errors'); fi;

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
	@$(call BLD, 'Linking ')
	@$(LD) -T $(SRCDIR)/ld.script -o $@ $(LDOPTS) $(OBJECTS) 2> temp.log || touch temp.errors
	@$(READELF) -a main > $(LISTINGSDIR)/program-elf.txt
	@$(call CHECK, $@:)
	@-$(RM) -f temp.errors temp.log

$(IMEMFILE): main
	@$(call BLD, 'Generating $@\n')
	@$(DUMP) -C -Dz --section=.text main > $(LISTINGSDIR)/program-listing.txt
	@cat $(LISTINGSDIR)/program-listing.txt | grep -P "^\s*[\d|abcdef]+:" | cut -f 2 > program.txt

$(DMEMFILE): main
	@$(call BLD, 'Generating $@\n')
	@-$(DUMP) -C -Dz --section=.data main > $(LISTINGSDIR)/data-listing.txt
	@-cat $(LISTINGSDIR)/data-listing.txt | grep -P "^\s*[\d|abcdef]+:" | cut -f 2 > data.txt

$(OBJDIR)/%.cc.o: $(SRCDIR)/%.cc
	@$(CC) $(CCOPTS) $< -o $@ 2> temp.log || touch temp.errors
	@$(CC) -MM -MT $(OBJDIR)/$*.cc.o $< -o $(OBJDIR)/$*.d
	@$(call CHECK, $<:)
	@-$(RM) -f temp.errors temp.log

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	@$(CC) $(CCOPTS) $< -o $@ 2> temp.log || touch temp.errors
	@$(CC) -MM -MT $(OBJDIR)/$*.c.o $< -o $(OBJDIR)/$*.d
	@$(call CHECK, $<:)
	@-$(RM) -f temp.errors temp.log

$(OBJDIR)/%.S.o: $(SRCDIR)/%.S
	@$(AS) -mips1 $< -o $@ 2> temp.log || touch temp.errors
	@$(call CHECK, $<:)
	@-$(RM) -f temp.errors temp.log

dirs: $(OBJDIR) $(LISTINGSDIR)

$(OBJDIR) $(LISTINGSDIR):
	@echo Creating directory $@
	@mkdir -p $@

clean:
	@echo Cleaning ...
	@-rm -rf $(OBJDIR)
	@-rm -rf $(LISTINGSDIR)
	@-rm main temp.log program.txt data.txt