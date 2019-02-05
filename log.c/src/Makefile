CC=gcc
CPPFLAGS=-std=gnu99 -Wall -Wextra -pedantic -g3 -c
LDFLAGS=-L.

LOG=log.o

all: $(LOG)

$(LOG): log.c
	$(CC) $(CPPFLAGS) -DLOG_USE_COLOR -DDEBUG $^ -o $@

clean: log.o
	rm log.o