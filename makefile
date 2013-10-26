CFLAGS=-Wall
LDFLAGS=

tam8085tape: tam8085tape.o

hello.wav: tam8085tape testprogram/hello.bin
	./tam8085tape testprogram/hello.bin - | sox -t raw --rate 32000 -b 8 -e signed-integer --channels 1 - hello.wav
