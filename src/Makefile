# Martin Vasko xvasko12
# FIT VUTBR 1.MSK
# Libnet makefile
# Date of creation = 22.9.2018
# Makefile usage = make, make all
# make pack, make clean

CC=gcc
CPPFLAGS=-std=gnu99 -Wall -Wextra -pedantic -pthread -g3
LDFLAGS=-L.

LOG=log.o
APP=libnetc
DBG=libnetcd
LIBOBJ=libnetc.o log.o

all: $(LOG) $(APP)

debug: $(DBG)

$(LOG): ../log.c/src/log.c
	cd ../log.c/src && $(MAKE)
	cp ../log.c/src/$(LOG) .

$(DBG): main.c $(LIBOBJ)
	$(CC) $(CPPFLAGS) -DDEBUG -g3 $^ -o $@

$(APP): main.c $(LIBOBJ)
	$(CC) $(CPPFLAGS) $^ -o $@

libnetc.o: libnetc.h libnetc.c
	$(CC) $(CPPFLAGS) -c libnetc.c

clean:
	cd ../log.c/src && $(MAKE) clean
	$(RM) $(APP) $(LIBOBJ) libnetc.zip

pack: $(wildcard *.c) $(wildcard *.h) Makefile
	zip -r libnetc.zip $@
