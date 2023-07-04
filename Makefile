CFLAGS=-std=c11 -g -static

mcc: mcc.cpp

test: mcc
	./test.sh

clean: 
	rm -f mcc *.o *~ tmp*