//references http://www.binarytides.com/server-client-example-c-sockets-linux/
/*
    C socket server example
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <openssl/aes.h>

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
    int ivClientFlag=0;
    unsigned char ivClient[8];
    //printf("---------------------daddress --------------,%s",daddress);
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];
    char client_msg_back[2000];	
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
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
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    while(1)
    {
	    AES_KEY aes_key;
	    struct ctr_state state;
	    //Accept and incoming connection
	    puts("Waiting for incoming connections...");
	    c = sizeof(struct sockaddr_in);
	     
	    //accept connection from an incoming client
	    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	    if (client_sock < 0)
	    {
		perror("accept failed");
		return 1;
	    }
	    puts("Connection accepted");
		
	    //Receive a message from client
		//memset(client_message,0,sizeof(client_message));
	    //memset(client_msg_back,0,sizeof(client_msg_back));
	    bzero(client_message,2000*sizeof(client_message[0]));
	    bzero(client_msg_back,2000*sizeof(client_msg_back[0]));
	    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
	    {

		    puts("Hi, this is server");
		
		    if(ivClientFlag==0)
		    {
			ivClientFlag=1;
			puts("iv  from client");
			puts(client_message);
			strcpy(ivClient,client_message);
			init_ctr_server(&state, ivClient);	

			if (!AES_set_encrypt_key(key, 128, &aes_key))
            		/* Handle the error */;
		    }
		    else
		    {
			puts("message from client");
			puts(client_message);
			AES_ctr128_encrypt(client_message, client_msg_back, strlen(client_message), &aes_key, state.ivec, state.ecount, &state.num);
			puts("decrypted message ");
			puts(client_msg_back);


		    puts("message from client");
		    puts(client_message);
		    int sock;
		    struct sockaddr_in server;
		    char message[1000] , server_reply[2000];
		     
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
		     
		    //keep communicating with server
		    //while(1)
		    //{
			
			 
			//Send some data
			puts("before send");

			if( send(sock , client_msg_back , strlen(client_msg_back) , 0) < 0)
			{
			    puts("Send failed");
			    return 1;
			}
			puts("return from send");
			 
			//Receive a reply from the server
			if( recv(sock , server_reply , 2000 , 0) < 0)
			{
			    puts("recv failed");
			    break;
			}
			 
			
		    
		     
		        puts("Server ----- reply :");
        		puts(server_reply);









			//Send the message back to client
			//write(client_sock , server_reply , strlen(server_reply));
			//printf("Reached here\n");
			//close(sock);
			


			//write(client_sock , client_msg_back , strlen(client_message));



		   //}
		   //close(sock);
			
		    }
	    //memset(client_message,0,sizeof(client_message));
	    //memset(client_msg_back,0,sizeof(client_msg_back));
	    bzero(client_message,2000*sizeof(client_message[0]));
	    bzero(client_msg_back,2000*sizeof(client_msg_back[0]));

			
	    }
	     
	    if(read_size == 0)
	    {
		puts("Client disconnected");
		fflush(stdout);
	    }
	    else if(read_size == -1)
	    {
		perror("recv failed");
	    }
    }
     
    return 0;
}
