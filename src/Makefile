# Martin Vasko xvasko12
# FIT VUTBR 1.MSK
# Libnet makefile
# Date of creation = 22.9.2018
# Makefile usage = make, make all
# make pack, make clean

CC=gcc
CPPFLAGS=-std=gnu99 -Wall -Wextra -pedantic -pthread
LDFLAGS=-L.

APP=libnetc
LIBOBJ=libnetc.o

all: $(APP)

$(APP): main.c $(LIBOBJ)
	$(CC) $(CPPFLAGS) $^ -o $@

libnetc.o: libnetc.h libnetc.c
	$(CC) $(CPPFLAGS) -c libnetc.c

clean:
	$(RM) $(APP) $(LIBOBJ) libnetc.zip

pack: $(wildcard *.c) $(wildcard *.h) Makefile
	zip -r libnetc.zip $@