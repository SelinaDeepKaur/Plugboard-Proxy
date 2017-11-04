/*
    C socket server example
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
 
int server(char *dAddress, char *dPort, char *serverPort)
{
    //printf("---------------------daddress --------------,%s",daddress);
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];
     
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
	    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
	    {
		
		    /*printf("message from client, %s",client_message);
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
			printf("before send");
			if( send(sock , client_message , strlen(client_message) , 0) < 0)
			{
			    puts("Send failed");
			    return 1;
			}
			printf("return from send");
			 
			//Receive a reply from the server
			if( recv(sock , server_reply , 2000 , 0) < 0)
			{
			    puts("recv failed");
			    break;
			}
			 
			
		    
		     
		        puts("Server ----- reply :");
        		puts(server_reply);









			//Send the message back to client
			write(client_sock , server_reply , strlen(server_reply));
			printf("Reached here\n");
			//close(sock);*/
			write(client_sock , client_message , strlen(client_message));
		   //}
		   //close(sock);
			bzero(client_message,2000*sizeof(client_message[0]));
			
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
