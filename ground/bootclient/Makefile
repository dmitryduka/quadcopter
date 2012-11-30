CC=g++
LIBS=-lboost_program_options-mt -lboost_system-mt -pthread -lbfd
CCOPTS=-I../../onboard/soft/src -I../common -O3 -std=c++0x

all:
	$(CC) $(CCOPTS) -c -o main.o main.cc
	$(CC) -O3 main.o $(LIBS) -o bootclient
clean:
	-rm *.o bootclient