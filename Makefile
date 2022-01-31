CFLAGS=-std=c11 -g -static

mcc: mcc.c

test: mcc
		./test.sh

clean:
		rm -f 9cc *.o *~ tmp*

.PHONY: test clean