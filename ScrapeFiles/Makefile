CC = gcc

CFLAGS = -I.
LIBS = -lcurl

DEPS = syscalls.h scraping.h
OBJ = curlFTP.o syscalls.o scraping.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

curlFTP: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJ)
