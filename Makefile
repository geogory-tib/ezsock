CC = cc
CCFLAGS = -Wall -Wextra
AR = ar
ARFLAGS = rcs 
clean:
	rm -rf *.o
shared:
	$(CC) $(CCFLAGS) -shared -fPIC -o libezsock.so ./src/*.c
static:
	$(CC) $(CCFLAGS) -c ./src/*.c
	$(AR)  libezsock.a *.o
debug:
	$(CC) $(CCFLAGS) -ggdb -c ./src/*.c
	$(AR) $(ARFLAGS)  libezsock.a *.o
