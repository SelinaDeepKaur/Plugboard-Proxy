//references http://www.binarytides.com/server-client-example-c-sockets-linux/

#include<stdio.h>
#include<string.h>    
#include <stdlib.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include "../includes/server.h"
#include "../includes/client.h"

struct ctr_state {
    unsigned char ivec[16];  
    unsigned int num;
    unsigned char ecount[16];
    };
int init_ctr_server(struct ctr_state *state, const unsigned char iv[8])
	{
	    state->num = 0;
	    memset(state->ecount, 0, 16);
	    memset(state->ivec + 8, 0, 8);
	    memcpy(state->ivec, iv, 8);
	    return 0;
	}

int maximum(int m, int n) {
	if (m > n)
		return m;
	return n;
}
 
int server(char *dAddress, char *dPort, char *serverPort, char *key)
{
    int n;
    int socket_desc , client_sock , c ;
    struct sockaddr_in server , client;

    AES_KEY aes_keyS, aes_keyC;
    int r;
    fd_set read_fds;
    struct ctr_state stateS, stateC;
    unsigned char ivClient[AES_BLOCK_SIZE], ivByServer[AES_BLOCK_SIZE];
    char message_client[4096];
    int ssh_sock;
    char server_reply[4096], client_message[4096], client_msg_back[4096];
    
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        fprintf(stderr,"Could not create socket");
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( atoi(serverPort) );
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        fprintf(stderr,"bind failed. Error");
        return 1;
    }

    
    c = sizeof(struct sockaddr_in);
     
    while(1)
    {
	listen(socket_desc , 3);
	fprintf(stderr,"\nWaiting for incoming connections...\n");
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0)
	{
		fprintf(stderr,"accept failed");
		return 1;
	}

			

	   
	ssh_sock = socket(AF_INET , SOCK_STREAM , 0);
	if (ssh_sock == -1)
	{
		fprintf(stderr,"Could not create socket");
	}    
	server.sin_addr.s_addr = inet_addr(dAddress);
	server.sin_family = AF_INET;
	server.sin_port = htons( atoi(dPort) );
	r=connect(ssh_sock , (struct sockaddr *)&server , sizeof(server));
	if (r < 0)
	{
		fprintf(stderr,"connect failed. Error");
		return 1;
	}

	/* Receiving the client's iv */
	memset(&ivClient[0],0,sizeof(ivClient));			
	r = read(client_sock , ivClient , AES_BLOCK_SIZE);
	if(r<0)
	{
	    fprintf(stderr,"Read iv from client failed");
	    return -1;
	}

	init_ctr_server(&stateC, ivClient);
	if((AES_set_encrypt_key((unsigned char *)key, 128, &aes_keyC))<0)
		fprintf(stderr,"\nproblem with AES set encrypt, server side\n");

	/* Server generating the iv and sending it to client */
	memset(&ivByServer[0], 0 , sizeof(ivByServer));
	if((RAND_bytes(ivByServer, AES_BLOCK_SIZE)<0))
	{
		fprintf(stderr,"\nError with RAND_bytes\n");
		return -1;
	}

	r = write(client_sock , ivByServer , AES_BLOCK_SIZE);
	if(r<0)
	{
	    fprintf(stderr,"Sending iv failed");
	    return -1;
	}
	
	init_ctr_server(&stateS, ivByServer);
	if((AES_set_encrypt_key((unsigned char *)key, 128, &aes_keyS))<0)
		fprintf(stderr,"\nproblem with AES set encrypt, server side\n");
	
	memset(&server_reply[0],0,sizeof(server_reply));
	memset(&message_client[0],0,sizeof(message_client));
	memset(&client_message[0],0,sizeof(client_message));
	memset(&client_msg_back[0],0,sizeof(client_msg_back));
	while(1) 
		{
			FD_ZERO(&read_fds);
			FD_SET(client_sock, &read_fds);
			FD_SET(ssh_sock, &read_fds);
			r = select(maximum(client_sock, ssh_sock)+1, &read_fds, NULL, NULL, NULL);

			if (FD_ISSET(client_sock, &read_fds)) 
			{		
			  
				n=read(client_sock , client_message , 4096);
				if(n>0)
				{	
					AES_ctr128_encrypt((unsigned char *)client_message, (unsigned char *)client_msg_back, n, &aes_keyC, stateC.ivec, stateC.ecount, &stateC.num);
					if( write(ssh_sock , client_msg_back , n) < 0)
					{
					    fprintf(stderr,"Send failed");
					    break;
					}
					usleep(15000);
					memset(&client_message[0],0,sizeof(client_message));
				     	memset(&client_msg_back[0],0,sizeof(client_msg_back));
				} 
				else 
				{
					close(ssh_sock);
					break;
				}
			}
			else if (FD_ISSET(ssh_sock, &read_fds)) 
				{
					n=read(ssh_sock , server_reply , 4096);
					if(n>0)
					{  
						AES_ctr128_encrypt((unsigned char *)server_reply, (unsigned char *)message_client, n, &aes_keyS, stateS.ivec, stateS.ecount, &stateS.num);
						if( write(client_sock , message_client, n)<0)
							break;
						usleep(15000);
						memset(&server_reply[0],0,sizeof(server_reply));
						memset(&message_client[0],0,sizeof(message_client));

					}
					else
					{
						close(ssh_sock);
						break;

					}
				}
		
		}
		
	}
     
    return 0;
}
