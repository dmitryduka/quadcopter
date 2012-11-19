CC=g++
LIBS=-lboost_program_options-mt -lboost_system-mt -pthread
CCOPTS=-O3 -std=c++0x

all:
	$(CC) $(CCOPTS) -c -o main.o main.cc
	$(CC) main.o $(LIBS) -o serialmux
clean:
	-rm *.o serialmux