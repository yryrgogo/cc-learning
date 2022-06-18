CFLAGS=-std=c11 -g -fno-common -Wall -Wno-switch
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

holycc: $(OBJS)
				$(CC) -o holycc $(OBJS) $(LDFLAGS)

$(OBJS): holycc.h

test: holycc
				./test.sh

debug: holycc
				./debug.sh

graph: holycc
				./graph.sh

clean:
				rm -rf holycc *.o *~ tmp*

.PHONY: test clean
