CC = g++-4.7 -std=c++11 -c
LD = g++-4.7
CCSOURCES = main.cc memmgr.cc
OBJECTS += $(CCSOURCES:.cc=.o)
OBJECTS += $(CSOURCES:.c=.o)

all: clean $(OBJECTS)
	$(LD) -o main $(OBJECTS)
.cc.o:
	$(CC) $(CCOPTS) $< -o $@

.c.o:
	$(CC) $(CCOPTS) $< -o $@

clean:
	-rm main program-objdump.txt program.txt *.o