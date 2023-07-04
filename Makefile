CFLAGS=-std=c11 -g -static

msc: msc.cpp

test: msc
	./test.sh

clean: 
	rm -f msc *.o *~ tmp*