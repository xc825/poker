all: poker

poker: poker.c txshldm.c
	gcc -Wall -I. -o poker poker.c txshldm.c -lm

clean:
	rm -f poker

