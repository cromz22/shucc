CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

shucc: $(OBJS)
	$(CC) -o shucc $(OBJS) $(LDFLAGS)

$(OBJS): shucc.h

test: shucc
	./test.sh

clean:
	rm -f shucc *.o *~ tmp*

.PHONY: test clean
