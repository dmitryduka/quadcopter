XTOOLS_LOCATION = ~/x-tools-new
XTOOLS_TRIPLE = mips-quadcopter-elf

AS   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-as
LD   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-g++
CC   = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-g++
DUMP = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-objdump
READELF = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-readelf
STRIP = $(XTOOLS_LOCATION)/$(XTOOLS_TRIPLE)/bin/$(XTOOLS_TRIPLE)-strip

GRN = tput setaf 2; echo $(1); tput sgr0
YLW = tput setaf 3; echo $(1); tput sgr0
RED = tput setaf 1; echo $(1); tput sgr0
BLD = tput bold; echo -n $(1); tput sgr0
CHECK = if test -e temp.errors; then $(call RED, $(1)) && cat temp.log; elif test -s temp.log; then $(call YLW, $(1)) && cat temp.log; else $(call GRN, '$(1) no warnings/errors'); fi;


SRCDIR = src
OBJDIR = obj
LISTINGSDIR = listings

PROGRAM = main
IMEMFILE = program.txt
DMEMFILE = data.txt

SRC_ROOT = src
DIRS := ${shell find ${SRC_ROOT} -type d -print}
OUTPUTDIRS = $(patsubst src/%, obj/%, $(DIRS))
SRC_AS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.S))
SRC_CC = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc) $(wildcard $(dir)/*.cpp))
SRC_C = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))

SRC_TO_OBJ = $(foreach file, $(1), $(patsubst src/%, obj/%, $(dir $(file)))$(notdir $(file)).o)

OBJECTS = $(call SRC_TO_OBJ, $(SRC_AS)) $(call SRC_TO_OBJ, $(SRC_C)) $(call SRC_TO_OBJ, $(SRC_CC))

DEPS = $(wildcard $(OBJDIR)/*.d)

STACK_SIZE = 2000
INCLUDEDIRS = $(addprefix -I, $(DIRS))
CCOPTS =  -c -std=c++11 -G0 $(INCLUDEDIRS)
CCOPTS += -Os
CCOPTS += -Wstack-usage=$(STACK_SIZE) 
CCOPTS += -flto -fdata-sections -ffunction-sections -fno-threadsafe-statics -fpack-struct -fstack-usage -fno-strict-aliasing -fno-exceptions -fno-rtti
LDOPTS = -Os -flto -Wl,--gc-sections -nostdlib -nostartfiles -fwhole-program -lgcc

all: dirs $(IMEMFILE) $(DMEMFILE)

include $(DEPS)

main: $(OBJECTS)
	@$(call BLD, 'Linking ')
	@$(LD) -Wl,-T,$(SRCDIR)/ld.script -o $@ $(OBJECTS) $(LDOPTS) 2> temp.log || touch temp.errors
	@$(call CHECK, $@:)
	@$(READELF) -a main > $(LISTINGSDIR)/program-elf.txt
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

dirs: $(OBJDIR) $(LISTINGSDIR) $(OUTPUTDIRS)

$(OBJDIR) $(LISTINGSDIR) $(OUTPUTDIRS):
	@mkdir -p $@

clean:
	@echo Cleaning ...
	@-rm -rf $(OBJDIR)
	@-rm -rf $(LISTINGSDIR)
	@-rm main temp.errors temp.log program.txt data.txt
