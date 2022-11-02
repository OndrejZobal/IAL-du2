CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -g3
FILES=hashtable.c test.c test_util.c

.PHONY: test clean

test: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

clean:
	rm -f test
