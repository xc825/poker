#Poker hands sorting

Program is written in C language.

Tested on Ubuntu 20.04 LTS

####Compiling

In folder ./poker run:

`$ make`


####Running

In folder _poker_ run

`$ ./poker`

To feed stream from file run

`$ cat ./test.txt | ./poker`

or

`$ cat ./testOmaha.txt | ./poker --omaha`

####Generating input lines
To generate file for input run:

`./dealer --lines=n >texas_input.txt`

`./dealer --lines=n --omaha >omaha_input.txt`

To pass generated inpt lines directly to poker program:

`./dealer --lines=n | ./poker`

`./dealer --lines=n --omaha | ./poker --omaha`

n - number of generated lines.

####Extra credits:
- implemented option `--omaha`

####Limitations
Max hands in Texas Holdem line - 23 hands.
Max hands in Omaha Holdem line - 11 hands

Caculating 25000 lines of random lenght (2-23 hands) Texas Holdem input on Intel Core i5 7th gen CPU takes ~ 15 seconds.
Calculating 25000 lines of radom lenght (2 - 11 hands) Omaha Holdem takes ~120 seconds. (240 seconds if all lines contains 11 hands)

Invalid input is not handled. Program will just crash.
After five first cards must exactry one space symbol.
Between hands is exactly one space symbol.
After each line exactly one new line symbol "\n".

Like in home task example (but without empty line between lines, I don't know how to write correct markdown):

4cKs4h8s7s Ad4s Ac4d As9s KhKd 5d6d

2h3h4h5d8d KdKs 9hJh


P.S. It works on my machine! :)