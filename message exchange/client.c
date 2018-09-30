#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno;

	struct sockaddr_in serv_addr;
	struct hostent *server;

	char sender[32];
	char receiver[32];
	char text[256];
	int n, option, c;

	if (argc < 3)
	{
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	printf("Connnecting to server...\n");
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	printf("Username:");
	bzero (sender, 32);
	fgets(sender,31,stdin);
	n = write(sockfd,sender,strlen(sender));
	if (n < 0) 
			error("ERROR writing to socket");
	
	bzero(text,256);
	n = read(sockfd,text,255);
	if (n < 0) 
		error("ERROR reading from socket");
	printf("%s\n",text);
	
	sleep(1);
	
	while(1)
	{
		printf("	");
		scanf("%d", &option);
		while (( c = getchar()) != '\n' && c != EOF) { } //clear input buffer

		//Convert from host to network byte order
		int converted = htonl(option);
		
		n = write(sockfd,&converted,sizeof(option));
		if (n < 0) 
			error("ERROR writing to socket");
		
		switch (option)
		{
			case 1:
				//Sender info
				//sender is autocompleted from server
				
				//Receiver info
				printf("Receiver: ");
				bzero (receiver, 32);
				fgets(receiver,31,stdin);
				n = write(sockfd,receiver,strlen(receiver));
				if (n < 0) 
					error("ERROR writing to socket");
				
				//Message
				printf("Message: ");
				bzero(text,256);
				fgets(text,255,stdin);
				n = write(sockfd,text,strlen(text));
				if (n < 0) 
					error("ERROR writing to socket");
				
				break;
				
			case 2:
				//Read input
				bzero(text,256);
				n = read(sockfd,text,255);
				if (n < 0) 
					error("ERROR reading from socket");
				
				//Repeat till ending signal is send
				while(!(strcmp(text, "done") == 0))
				{
					//Show input
					printf("%s\n",text);
					fflush(stdout);
					
					bzero(text,256);
					n = read(sockfd,text,255);
					if (n < 0) 
					error("ERROR reading from socket");
					
					usleep(500);
				}
				
				break;
		
			case 3:
				printf("Bye Bye\n");
				return 0;
		}
		
	}
	return 0;
}
