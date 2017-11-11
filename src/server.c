//references http://www.binarytides.com/server-client-example-c-sockets-linux/
/*
    C socket server example
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include <stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <openssl/aes.h>
#include <fcntl.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/buffer.h>
struct ctr_state {
    unsigned char ivec[16];  /* ivec[0..7] is the IV, ivec[8..15] is the big-endian counter */
    unsigned int num;
    unsigned char ecount[16];
    };
int init_ctr_server(struct ctr_state *state, const unsigned char iv[8])
	{
	    /* aes_ctr128_encrypt requires 'num' and 'ecount' set to zero on the
	     * first call. */
	    state->num = 0;
	    memset(state->ecount, 0, 16);

	    /* Initialise counter in 'ivec' to 0 */
	    memset(state->ivec + 8, 0, 8);

	    /* Copy IV into 'ivec' */
	    memcpy(state->ivec, iv, 8);
	}
 
int server(char *dAddress, char *dPort, char *serverPort, char *key)
{
    //unsigned char ivS[8];
    int ivClientFlag=0;
    int ivServerFlag=0;
    unsigned char ivClient[8];
    //char *ivClient = "abc";
    char message_client[4096];
    int ssh_sock;
    char message[4096] , server_reply[4096];
    int n;
    //printf("---------------------daddress --------------,%s",daddress);
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[4096];
    char client_msg_back[4096];	

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        fprintf(stderr,"Could not create socket");
    }
    fprintf(stderr,"Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    //server.sin_addr.s_addr = inet_addr(dAddress);
    server.sin_addr.s_addr = INADDR_ANY;
    //printf("-----------------------dport----------------------%s",dPort);
    server.sin_port = htons( atoi(serverPort) );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    fprintf(stderr,"bind done");
     
    //Listen
    listen(socket_desc , 3);

    c = sizeof(struct sockaddr_in);
     
    while(1)
    {
	    AES_KEY aes_key;
	    struct ctr_state state;
	    
	    fprintf(stderr,"Waiting for incoming connections...");
	    
	     
	    
	    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	    if (client_sock < 0)
	    {
		perror("accept failed");
		return 1;
	    }
	    fprintf(stderr,"Connection accepted");
	    
		
	    
	    memset(&client_message[0],0,sizeof(client_message));
	    memset(&client_msg_back[0],0,sizeof(client_msg_back));
	    
	    
		
	    ssh_sock = socket(AF_INET , SOCK_STREAM , 0);
	    if (ssh_sock == -1)
	    {
		fprintf(stderr,"Could not create socket");
	    }
	    fprintf(stderr,"Socket created");
	     
	    
	    server.sin_addr.s_addr = inet_addr(dAddress);
            //server.sin_addr.s_addr = inet_addr("127.0.0.1");
	    server.sin_family = AF_INET;
	    server.sin_port = htons( atoi(dPort) );
	 
	    //Connect to remote server
	    if (connect(ssh_sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	    {
		perror("connect failed. Error");
		return 1;
	    }
	    
		    //if(ivClientFlag==0)
		    //	    {
				
				read(client_sock , ivClient , 4096);
		    		   
				//ivClientFlag=1;
				
				//read(client_sock , ivClient , 4096);
				//fprintf(stderr,client_message);
				//fprintf(stderr,"\nServer reveived iv  from client %s\n",client_message);
				//strcpy(ivClient,client_message);
				fprintf(stderr,"\nServer reveived iv  from client %s  \n",ivClient);
				//fprintf(stderr,"\nServer side: inside if clause for iv\n");
				//memset(&client_message[0],0,4096);

				/*init_ctr_server(&state, ivClient);
				if((AES_set_encrypt_key(key, 128, &aes_key))<0)
					fprintf(stderr,"\nproblem with AES set encrypt, server side\n");*/
				
				//continue;
		//	    }
	     
	    fprintf(stderr,"Connected\n");
	    fcntl(client_sock, F_SETFL, O_NONBLOCK);
	    fcntl(ssh_sock, F_SETFL, O_NONBLOCK);
	    
	init_ctr_server(&state, ivClient);
		if((AES_set_encrypt_key(key, 128, &aes_key))<0)
		fprintf(stderr,"\nproblem with AES set encrypt, server side\n");
	    
	    
	    while(1) {


		    while( (n=read(client_sock , client_message , 4096)) > 0 )
		    {   

			    fprintf(stderr,"Hi, this is server");

			        fprintf(stderr,"\nServer side: inside else clause of iv\n");
				//puts("key");
				//puts(key);
					
				fprintf(stderr,"key = %s\n",key);
				fprintf(stderr,"encrypted message from client = %s\n",client_message);
				/*init_ctr_server(&state, ivClient);
				if((AES_set_encrypt_key(key, 128, &aes_key))<0)
					fprintf(stderr,"\nproblem with AES set encrypt, server side\n");*/
				
				AES_ctr128_encrypt(client_message, client_msg_back, n, &aes_key, state.ivec, state.ecount, &state.num);
				fprintf(stderr,"decrypted message from client = %s\n",client_msg_back);
				//fprintf(stderr,"message from client ");
				//fprintf(stderr,client_msg_back);
				//socket



				 
				//Send some data
				//puts("before send");

				if( write(ssh_sock , client_msg_back , n) < 0)
				{
				    fprintf(stderr,"Send failed");
				    return 1;
				}
				//puts("return from send");
				memset(&client_message[0],0,sizeof(client_message));
			     	memset(&client_msg_back[0],0,sizeof(client_msg_back));
			     
			     
			}	 
				//Receive a reply from the server
			while((n=read(ssh_sock , server_reply , 4096)) > 0)
			{
				  
				fprintf(stderr,"Ssh----- reply %s :",server_reply);
				//puts("before sending back to client\n");
				/*init_ctr_server(&state, ivClient);
				if((AES_set_encrypt_key(key, 128, &aes_key))<0)
					fprintf(stderr,"\nproblem with AES set encrypt, server side\n");*/
				AES_ctr128_encrypt(server_reply, message_client, n, &aes_key, state.ivec, state.ecount, &state.num);
				write(client_sock , message_client, n);
				memset(&server_reply[0],0,4096);
				memset(&message_client[0],0,4096);
				//puts("after sending back to client\n");

			}
		
		//ivClientFlag=0;
	    	}
		
		
    }
     
    return 0;
}
