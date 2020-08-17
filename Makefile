CFLAGS=-std=c11 -g -static

shucc: shucc.c

test: shucc
	./test.sh

clean:
	rm -f shucc *.o *~ tmp*

.PHONY: test clean
