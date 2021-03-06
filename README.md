# Plugboard-Proxy

a) Test Environment:

	Description:	Ubuntu 16.04.3 LTS
	Release:	16.04
	Codename:	xenial

b) Compiler and its version:

	gcc (Ubuntu 5.4.0-6ubuntu1~16.04.5) 5.4.0 20160609

c) Command line with the options (both server and client):

	Server Side :  ./bin/pbproxy -l 2222 -k mykey localhost 22
	Client Side :  ssh -o "ProxyCommand ./bin/pbproxy -k mykey localhost 2222" localhost


d) Design of pbproxy:

	Files:
	1)src/pbproxy.c 

	Takes in the command with the options [ -l, -k etc ] (-k option provides the key file named "mykey" from which the key will   be read). 
	(The key that is read from the file is a hexadecimal string. Each string byte (eg 'FF') is converted into an actual byte       value).
	Converts hostnames to ip
	Runs the server if -l option is supplied
	Runs the client if -l option is not supplied

	2)src/client.c

	Client establishes connection with the server.
	Sends iv to the server.
	Goes into an infinite loop to keep sending messages to the server from STDIN and to keep printing messages from the server     to STDOUT.
	It also receives an iv from the server.
	It accepts a message in plain text format from STDIN, encrypts it and sends the encrypted messsage to the server.
	Similarly it decrypts the encrypted messages that it receives from the server, using the iv that the server sent, before       printing them to STDOUT.
	The send and receive operations happen asynchronously.

	3)src/server.c

	Keeps accepting connections from varied clients in an infinite loop.
	Receives an iv from the client.
	Generates and sends and iv to the client.
	Once a connection with a client is established, goes into an infinite loop to keep receiving messages from the client. 
	Once it receives a message from a client, it decrypts it using the clients iv and relays it to some destination service.
	Once receives the reply from the service, encrypts it using its own iv and sends it back to the client.
	The send and receive operations happen asynchronously.

e) References: 

	http://www.binarytides.com/server-client-example-c-sockets-linux/
	https://stackoverflow.com/questions/3141860/aes-ctr-256-encryption-mode-of-operation-on-openssl
	https://notes.shichao.io/unp/ch6/

