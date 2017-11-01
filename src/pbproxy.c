#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../includes/server.h"
#include "../includes/client.h"
int main(int argc, char **argv)
	{
	    char *serverPort=NULL, *keyfile=NULL, *daddress=NULL, *dport=NULL;
	    int c, index;
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
	    daddress =argv[index++];
	    dport = argv[index];
    	    
	    if(keyfile==NULL || daddress == NULL || dport == NULL)
	    {
		printf("Keyfile or destination address or destination port not specified\n");
                exit(0);
    	    }
	    
	    
	    /*printf("-l %s",serverPort);
	    printf("-k %s",keyfile);
	    printf("-daddress %s",daddress);
            printf("-dport %s",dport);*/
	    if(serverPort !=NULL)
	    {
		server();
	    }
	    else
	    {
		client();
	    }
	
	    
	    return 0;
	}
		
