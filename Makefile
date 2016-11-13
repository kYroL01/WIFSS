# Horloge's Makefile for: WIFSS

CC=gcc
CFLAGS=-D _XOPEN_SOURCE=200809L -std=c99 -O3 -pthread -Wall -Wextra -Wpedantic -Wshadow -Wno-missing-field-initializers -Wstrict-overflow
LDFLAGS=-pthread
EXEC=WClient WServer


# By default, build both Client and Server
all: $(EXEC)


# Client
WClient: ccommands.o ccore.o cclient.o cfiles.o cprotocols.o
	$(MAKE) str.o
	$(CC) -o $@ $^ str.o $(LDFLAGS)
	$(MAKE) clean

ccommands.o: client/commands/*
	$(CC) -o $@ -c client/commands/commands.c $(CFLAGS)
ccore.o: client/core/*
	$(CC) -o $@ -c client/core/core.c $(CFLAGS)
cclient.o: client/core/*
	$(CC) -o $@ -c client/core/client.c $(CFLAGS)
cfiles.o: client/files/*
	$(CC) -o $@ -c client/files/files.c $(CFLAGS)
cprotocols.o: client/protocols/*
	$(CC) -o $@ -c client/protocols/protocols.c $(CFLAGS)


# Server
WServer: scommands.o score.o sserver.o sprotocols.o
	$(MAKE) str.o
	$(CC) -o $@ $^ str.o $(LDFLAGS)
	$(MAKE) clean

scommands.o: server/commands/*
	$(CC) -o $@ -c server/commands/commands.c $(CFLAGS)
score.o: server/core/*
	$(CC) -o $@ -c server/core/core.c $(CFLAGS)
sserver.o: server/core/*
	$(CC) -o $@ -c server/core/server.c $(CFLAGS)
sprotocols.o: server/protocols/*
	$(CC) -o $@ -c server/protocols/protocols.c $(CFLAGS)


# Common string functions
str.o: wifss/str/*
	$(CC) -o $@ -c wifss/str/str.c $(CFLAGS)


.SILENT:
.PHONY: clean mrproper

clean:
	@rm -f *.o

mrproper: clean
	@rm -f $(EXEC)
