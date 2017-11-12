#include<stdio.h> 
#include<string.h>    
#include <stdlib.h>
#include<sys/socket.h>    
#include<arpa/inet.h> 
#include <openssl/aes.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include <sys/select.h>
#include <sys/time.h>

int client(char*, char*, char*);
