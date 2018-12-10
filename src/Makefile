# Martin Vasko xvasko12
# FIT VUTBR 1.MSK
# Libnet makefile
# Date of creation = 22.9.2018
# Makefile usage = make, make all
# make pack, make clean

CC=gcc
CPPFLAGS=-std=gnu99 -Wall -Wextra -pedantic -pthread -g3
LDFLAGS=-L.

APP=libnetc
DBG=libnetcd
LIBOBJ=libnetc.o

all: $(APP)

debug: $(DBG)

$(DBG): main.c $(LIBOBJ)
	$(CC) $(CPPFLAGS) -DDEBUG -g3 $^ -o $@

$(APP): main.c $(LIBOBJ)
	$(CC) $(CPPFLAGS) $^ -o $@

libnetc.o: libnetc.h libnetc.c
	$(CC) $(CPPFLAGS) -c libnetc.c

clean:
	$(RM) $(APP) $(LIBOBJ) libnetc.zip

pack: $(wildcard *.c) $(wildcard *.h) Makefile
	zip -r libnetc.zip $@
