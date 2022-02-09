CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

mcc: $(OBJS)
		$(CC) -o mcc $(OBJS) $(LDFLAGS)

$(OBJS): mcc.h

test: mcc
		./test.sh

clean:
		rm -f 9cc *.o *~ tmp*

.PHONY: test clean