//References https://stackoverflow.com/questions/3141860/aes-ctr-256-encryption-mode-of-operation-on-openssl
/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <openssl/aes.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
 
struct ctr_state {
    unsigned char ivec[16];  /* ivec[0..7] is the IV, ivec[8..15] is the big-endian counter */
    unsigned int num;
    unsigned char ecount[16];
    };
int init_ctr(struct ctr_state *state, const unsigned char iv[8])
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

int client(char *dAddress, char *dPort, char *key)
{
    
	int n;
	int ivServerFlag=0;
	//unsigned char iv[8];
	char *iv="abc";
	unsigned char ivServer[8];
	struct ctr_state state;

	int sock;
	struct sockaddr_in server;
	char message[4096] , server_reply[4096], msg_out[4096], final_message[4096];

	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
	fprintf(stderr,"Could not create socket");
	}
	fprintf(stderr,"Socket created");

	server.sin_addr.s_addr = inet_addr(dAddress);
	//server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( atoi(dPort) );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	fcntl(sock, F_SETFL, O_NONBLOCK);

	fprintf(stderr,"Connected\n");

	//RAND_bytes(iv, 8)
	/* Handle the error */;
	AES_KEY aes_key;

	/*init_ctr(&state, iv);	
	
	AES_set_encrypt_key(key, 128, &aes_key);*/
           
	/*if( send(sock , iv , 8 , 0) < 0)
	{
	    //puts("Sending iv failed");
	    return 1;
	}
	fprintf(stderr,"\n Client side iv =%s\n",iv);*/
	//puts(iv);
	
	init_ctr_server(&state, iv);	
		//fprintf(stderr,"key = %s\n",key);
	if((AES_set_encrypt_key(key, 128, &aes_key))<0)
		fprintf(stderr,"\nproblem with AES set encrypt, server side\n");

	while(1)
	{    


		while((n=read(STDIN_FILENO,message, 4096))>0)
		{
			fprintf(stderr,"sending message = %s\n",message);
			fprintf(stderr,"key = %s\n",key);
			
			fprintf(stderr,"client side: sending message %s\n",message);
			AES_ctr128_encrypt(message, msg_out, n, &aes_key, state.ivec, state.ecount, &state.num);

			fprintf(stderr,"encrypted message %s\n",msg_out);

	

			if( write(sock , msg_out , n) < 0)
			{
			    fprintf(stderr,"Send failed");
			    return 1;
			}
			memset(&message[0], 0 , sizeof(message));
			memset(&msg_out[0], 0 , sizeof(msg_out));
	
		}

		//fprintf(stderr,"Waiting");
		while( (n=read(sock , server_reply , 4096)) > 0)
		{

				fprintf(stderr,"\nServer reply : %s\n",server_reply);
				AES_ctr128_encrypt(server_reply, final_message, n, &aes_key, state.ivec, state.ecount, &state.num);
				//puts("-------Server Reply --------");
				write(STDOUT_FILENO, final_message, n);
				memset(&server_reply[0],0,4096);
				memset(&final_message[0],0,4096);
		}


	}
     
    close(sock);
    return 0;
}
