CC = gcc
SOURCE= src/pbproxy.c
SERVER= src/server.c
CLIENT= src/client.c
OUTPUT= bin/pbproxy

all:
	$(CC) $(SERVER) $(CLIENT) $(SOURCE) -o $(OUTPUT) -l crypto -Wall -Werror
clean:
	rm -vf $(OUTPUT)
