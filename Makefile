all:test
test:	main.o memkit.o
	gcc $^ -o $@ $(LDFLAGS)
	
LDFLAGS+=-lpthread
clean:
	rm *.o
	rm ./test
	rm *.d

.PHONY:
	clean
sources = *.c


include $(sources:.c=.d)

%.d:%.c
	set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	rm *.d
