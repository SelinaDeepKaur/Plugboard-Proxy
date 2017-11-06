//References https://stackoverflow.com/questions/3141860/aes-ctr-256-encryption-mode-of-operation-on-openssl
/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <openssl/aes.h>
 
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
    

	
	unsigned char iv[8];
	struct ctr_state state;

	if (!RAND_bytes(iv, 8))
	    /* Handle the error */;

	init_ctr(&state, iv);	
    	AES_KEY aes_key;

	if (!AES_set_encrypt_key(key, 128, &aes_key))
            /* Handle the error */;

    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000], msg_out[1000];
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
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
     
    puts("Connected\n");
    puts("iv ");
    puts(iv);
	
    if( send(sock , iv , strlen(iv) , 0) < 0)
        {
            puts("Sending iv failed");
            return 1;
        }
    //keep communicating with server
    while(1)
    {
	
	memset(message, 0 , sizeof(message));
	memset(msg_out, 0 , sizeof(msg_out));
	bzero(message,2000*sizeof(message[0]));
	bzero(msg_out,2000*sizeof(msg_out[0]));
        printf("Enter message : ");
	scanf("%s", message);
	puts(message);
	
	AES_ctr128_encrypt(message, msg_out, strlen(message), &aes_key, state.ivec, state.ecount, &state.num);
	puts(msg_out);
        if( send(sock , msg_out , strlen(msg_out) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        puts("Waiting");
        //Receive a reply from the server
	bzero(server_reply,2000*sizeof(server_reply[0]));
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
         
        puts("\nServer reply :");
        puts(server_reply);
	
    }
     
    close(sock);
    return 0;
}
