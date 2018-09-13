all:test
test:	main.o memkit.o
	g++ $^ -o $@ $(LDFLAGS)
	
LDFLAGS+=-lpthread
clean:
	rm *.o
	rm ./test
	rm *.d

.PHONY:
	clean
sources = *.c


include $(sources:.c=.d)
CC=g++

%.d:%.c
	set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	rm *.d
