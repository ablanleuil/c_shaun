CC=gcc
CFLAGS=-O3 -I./include
LDFLAGS=-L./lib

LIB_SRC=$(wildcard src/*.c)
LIB_OBJ=$(patsubst src/%.c,obj/%.o,$(LIB_SRC))

clean:
	rm obj/*
	rm bin/*
	rm lib/*

all: lib/libshaun.so bin/parse_file

parse_file: bin/parse_file
bin/parse_file: obj/parse_file.o
	$(CC) $(LDFLAGS) -lshaun -o $@ $^

lib/libshaun.so: $(LIB_OBJ)
	$(CC) $(LDFLAGS) -fPIC -shared -o $@ $^

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

obj/parse_file.o: examples/parse_file.c
	$(CC) $(CFLAGS) -c -lshaun -o $@ $^
