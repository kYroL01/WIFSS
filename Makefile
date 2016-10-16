# Horloge's Makefile for: WIFSS

CC=gcc
CFLAGS=-ansi -D _POSIX_C_SOURCE=200112L -std=c99 -O3 -pthread -Wall -Wextra -Wpedantic -Wshadow -Wno-missing-field-initializers -Wstrict-overflow
LDFLAGS=
EXEC=WClient WServer


# By default, build both Client and Server
all: $(EXEC)


# Client
WClient: ccommands.o ccore.o cclient.o cfiles.o cprotocols.o
	$(MAKE) str.o
	$(CC) -o $@ $^ str.o $(CFLAGS) $(LDFLAGS)
	$(MAKE) clean

ccommands.o: client/commands/*
	$(CC) -o $@ -c client/commands/commands.c $(CFLAGS) $(LDFLAGS)
ccore.o: client/core/*
	$(CC) -o $@ -c client/core/core.c $(CFLAGS) $(LDFLAGS)
cclient.o: client/core/*
	$(CC) -o $@ -c client/core/client.c $(CFLAGS) $(LDFLAGS)
cfiles.o: client/files/*
	$(CC) -o $@ -c client/files/files.c $(CFLAGS) $(LDFLAGS)
cprotocols.o: client/protocols/*
	$(CC) -o $@ -c client/protocols/protocols.c $(CFLAGS) $(LDFLAGS)


# Server
WServer: scommands.o score.o sserver.o sprotocols.o
	$(MAKE) str.o
	$(CC) -o $@ $^ str.o $(CFLAGS) $(LDFLAGS)
	$(MAKE) clean

scommands.o: server/commands/*
	$(CC) -o $@ -c server/commands/commands.c $(CFLAGS) $(LDFLAGS)
score.o: server/core/*
	$(CC) -o $@ -c server/core/core.c $(CFLAGS) $(LDFLAGS)
sserver.o: server/core/*
	$(CC) -o $@ -c server/core/server.c $(CFLAGS) $(LDFLAGS)
sprotocols.o: server/protocols/*
	$(CC) -o $@ -c server/protocols/protocols.c $(CFLAGS) $(LDFLAGS)


# Common string functions
str.o: wifss/str/*
	$(CC) -o $@ -c wifss/str/str.c $(CFLAGS) $(LDFLAGS)


.SILENT:
.PHONY: clean mrproper

clean:
	@rm -f *.o

mrproper: clean
	@rm -f $(EXEC)
