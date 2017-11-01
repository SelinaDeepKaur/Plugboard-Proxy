CC = gcc
SOURCE= src/pbproxy.c
SERVER= src/server.c
CLIENT= src/client.c
OUTPUT= bin/pbproxy

all:
	$(CC) $(SERVER) $(CLIENT) $(SOURCE) -o $(OUTPUT) 
clean:
	rm -vf $(OUTPUT)
