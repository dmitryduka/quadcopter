CC = g++ -std=c++0x -g3 -O0 -c
LD = g++
CCSOURCES = main.cc float32.cc
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