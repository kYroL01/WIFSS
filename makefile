SERVER ?= @gcc ./server/src/*.c -lpthread -I ./server/inc/ -o ./WServer -Wall -ansi -D _XOPEN_SOURCE
CLIENT ?= @gcc ./client/src/*.c -lpthread -I ./client/inc/ -o ./WClient -Wall -ansi -D _XOPEN_SOURCE


all:

	$(SERVER)
	$(CLIENT)

SERVER:

	$(SERVER)

CLIENT:

	$(CLIENT)
