all: poker dealer

poker: poker.c txshldm.c
	gcc -Wall -I. -o poker poker.c txshldm.c

dealer: dealer.c
	gcc -Wall -I. -o dealer dealer.c

clean:
	rm -f poker dealer

