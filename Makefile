CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
CC=g++

mcc: $(OBJS)
	$(CC) -o mcc $(OBJS) $(LDFLAGS)

$(OBJS): generator.hpp parser.hpp tokenizer.hpp types.hpp

test: mcc
	./test.sh

clean: 
	rm -f mcc *.o *~ tmp*

.PHONY: test clean