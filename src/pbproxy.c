//references http://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include<sys/socket.h>
#include<errno.h> 
#include<netdb.h> 
#include<arpa/inet.h>
#include "../includes/server.h"
#include "../includes/client.h"

int hostname_to_ip(char *hostname , char*ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;     
    if((he = gethostbyname( hostname ) ) == NULL) 
    {
        herror("gethostbyname");
        return 1;
    }
    addr_list =(struct in_addr **)he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        strcpy(ip, inet_ntoa(*addr_list[i]) );
        return 0;
    }
     
    return 1;
}
int main(int argc, char **argv)
	{
	    char *serverPort=NULL, *keyfile=NULL, *dAddress=NULL, *dPort=NULL;
	    int c, index;
	    char ip[100];
	    while ((c = getopt (argc, argv, "l:k:")) != -1)
	    switch (c)
	    {
		case 'l':
                    serverPort = (optarg);
                    break;
            	case 'k':
                    keyfile = (optarg);
                    break;
		default:
		    printf("Input not supported\n");
                    exit(0);
	    }
	    index = optind;
	    dAddress =argv[index++];
	    dPort = argv[index];
    	    
	    if(keyfile==NULL || dAddress == NULL || dPort == NULL)
	    {
		printf("Keyfile or destination address or destination port not specified\n");
                exit(0);
    	    }
	    hostname_to_ip(dAddress , ip);
	    FILE *p;
   	    p = fopen(keyfile, "r");
	    char key[1000]={0};
	    if(p)
	    {
		fgets(key, 1000,(FILE*)p);
	    }
	    else
	    {
	    	fprintf(stderr,"file not found");
		exit(0);
	    }

	    if(serverPort !=NULL)
	    {
		server(ip,dPort,serverPort,key);
	    }
	    else
	    {
		client(ip,dPort,key);
	    }
	
	    
	    return 0;
	}
		
