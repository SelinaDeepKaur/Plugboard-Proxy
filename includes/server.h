#include<stdio.h>
#include<string.h>    
#include <stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/buffer.h>

int server(char* a, char* b, char* c, char* d);

