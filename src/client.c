//References https://stackoverflow.com/questions/3141860/aes-ctr-256-encryption-mode-of-operation-on-openssl
#include <stdio.h> 
#include <string.h>    
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>    
#include <arpa/inet.h> 
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

#include "../includes/server.h"
#include "../includes/client.h"

struct ctr_state {
    unsigned char ivec[16];  
    unsigned int num;
    unsigned char ecount[16];
    };
int init_ctr(struct ctr_state *state, const unsigned char iv[8])
	{
	    
	    state->num = 0;
	    memset(state->ecount, 0, 16);
	    memset(state->ivec + 8, 0, 8);    
	    memcpy(state->ivec, iv, 8);
	    return 0;
	}

int client(char *dAddress, char *dPort, char *key)
{
    
	int n, sock, r;
	fd_set read_fds;
	unsigned char iv[AES_BLOCK_SIZE], ivServer[AES_BLOCK_SIZE];
	struct ctr_state stateC, stateS;
	AES_KEY aes_keyC, aes_keyS;
	struct sockaddr_in server;
	char message[4096] , server_reply[4096], msg_out[4096], final_message[4096];
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		fprintf(stderr,"Could not create socket");
	}
	server.sin_addr.s_addr = inet_addr(dAddress);
	server.sin_family = AF_INET;
	server.sin_port = htons( atoi(dPort) );

	r = connect(sock , (struct sockaddr *)&server , sizeof(server));
	if (r < 0)
	{
		fprintf(stderr,"connect failed. Error");
		return 1;
	}
	/* Creating iv */
	memset(&iv[0], 0 , sizeof(iv));
	if((RAND_bytes(iv, AES_BLOCK_SIZE)<0))
		{
		fprintf(stderr,"\nError with RAND_bytes\n");
		return -1;
		}
	
	/* Sending iv to Server */
	r= write(sock , iv , AES_BLOCK_SIZE);
	
	if(r<0)
	{
	    fprintf(stderr,"Sending iv failed");
	    return -1;
	}

	init_ctr(&stateC, iv);		
	if((AES_set_encrypt_key((unsigned char *)key, 128, &aes_keyC))<0)
		{
		fprintf(stderr,"\nproblem with AES set encrypt function\n");
		return -1;
		}

	/* Receiving the server's iv */
	memset(&ivServer[0],0,sizeof(ivServer));			
	r = read(sock , ivServer , AES_BLOCK_SIZE);
	if(r<0)
	{
	    fprintf(stderr,"Reading iv from server failed");
	    return -1;
	}
	init_ctr(&stateS, ivServer);		
	if((AES_set_encrypt_key((unsigned char *)key, 128, &aes_keyS))<0)
		{
		fprintf(stderr,"\nproblem with AES set encrypt function\n");
		return -1;
		}
	


	memset(&message[0], 0 , sizeof(message));
	memset(&msg_out[0], 0 , sizeof(msg_out));
	memset(&server_reply[0],0,sizeof(server_reply));
	memset(&final_message[0],0,sizeof(final_message));
	while(1)
	{    
		FD_ZERO(&read_fds);
		FD_SET(STDIN_FILENO, &read_fds);
		FD_SET(sock, &read_fds);
		r = select(sock+1, &read_fds, NULL, NULL, NULL);


		if (FD_ISSET(STDIN_FILENO, &read_fds)) 
		{
			n= read(STDIN_FILENO,message, 4096);
			if(n<0)
			{
			    fprintf(stderr,"Read from STDIN failed");
			    break;
			}
			AES_ctr128_encrypt((unsigned char *)message, (unsigned char *)msg_out, n, &aes_keyC, stateC.ivec, stateC.ecount, &stateC.num);		
			if(write(sock , msg_out , n) < 0)
			{
			    fprintf(stderr,"Send failed");
			    break;
			}
			usleep(15000);
			memset(&message[0], 0 , sizeof(message));
			memset(&msg_out[0], 0 , sizeof(msg_out));
		}
		else if (FD_ISSET(sock, &read_fds)) 
		{	
			n=read(sock , server_reply , 4096);
			if(n<0)
			{
			    fprintf(stderr,"Read from server failed");
			    break;
			}

			AES_ctr128_encrypt((unsigned char *)server_reply, (unsigned char *)final_message, n, &aes_keyS, stateS.ivec, stateS.ecount, &stateS.num);
			n = write(STDOUT_FILENO, final_message, n);
			usleep(15000);
			if(n<0)
			{
			    fprintf(stderr,"Write to STDOUT failed");
			    break;
			}
			memset(&server_reply[0],0,sizeof(server_reply));
			memset(&final_message[0],0,sizeof(final_message));	
		}

	}
     
    close(sock);
    return 0;
}
