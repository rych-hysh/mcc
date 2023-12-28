CPPFLAGS=-std=c++17 -g -static -Wall
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
CXX=g++

mcc: $(OBJS)
	$(CXX) -o mcc $(OBJS) $(LDFLAGS) 

$(OBJS): generator.hpp parser.hpp tokenizer.hpp types.hpp

test: mcc
	./test.sh

clean: 
	rm -f mcc *.o *~ tmp* *.log *.md parser tokenizer util generator test

.PHONY: test clean