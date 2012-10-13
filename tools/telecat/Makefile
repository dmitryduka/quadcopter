
all:
	g++-4.7 -std=c++11 -I../../onboard/soft/src/ -c -o float32.o ../../onboard/soft/src/system/fp/float32.cc
	g++-4.7 -std=c++11 -I../../onboard/soft/src/ -c -o main.o main.cc
	g++-4.7 -o main main.o float32.o

clean:
	-rm main.o main