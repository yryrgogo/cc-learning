CFLAGS=-std=c11 -g -fno-common -Wall -Wno-switch
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

holycc: $(OBJS)
				$(CC) -o holycc $(OBJS) $(LDFLAGS)

$(OBJS): holycc.h

test: holycc
				./test.sh

debug: holycc
				./holycc ./test/debug_source.c > tmp.s
				cc -o tmp tmp.s ./sample/call_func.c pointer_calc.o
				./tmp

graph: holycc
				./graph.sh

clean:
				rm -rf holycc *.o *~ tmp*

.PHONY: test clean
