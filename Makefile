CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

shucc: $(OBJS)
	$(CC) -o shucc $(OBJS) $(LDFLAGS)

$(OBJS): shucc.h

test: shucc test/test.c test/util.o
	./shucc test/test.c > test/tmp.s
	gcc -static -o test/tmp test/tmp.s test/util.o
	./test/tmp

test/test.c: prep.sh
	./prep.sh > $@

test/util.o: test/util.c
	gcc -xc -c -o test/util.o test/util.c

clean:
	rm -f shucc *.o *~ tmp*

.PHONY: test clean
