//http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html

//printf functions that are commented out are used for error checking and verifaction that the programm works

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "list.h" //contains the linked list implementation

void *chat (void *);
	
void error(char *msg)
{
	perror(msg);
	exit(1);
}

static struct node* head = NULL;
struct node* temp = NULL;

int main(int argc, char *argv[])
{	
	int sockfd, newsockfd, portno, clilen, threadid;
	pthread_t client_request;
	
	struct sockaddr_in serv_addr, cli_addr;

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	
	//Initializing the list. Making an "invisible" first element to avoid the list being empty.
	DATA ini;
	strncpy(ini.sender, "0", sizeof(ini.sender));
	strncpy(ini.receiver, "0", sizeof(ini.receiver));
	strncpy(ini.text, "0", sizeof(ini.text));
	init(&head, ini);
	
	while (1)
	{		
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");
		if (pthread_create(&client_request, NULL, chat, (void *) newsockfd) < 0)
			error("ERROR on creating thread");
		pthread_detach(pthread_self());
	}
	pthread_exit(NULL);
	return 0; 
}

void *chat (void *sockfd)
{
	int newsockfd = (int)sockfd;
	int n, option;
	int found = 0; //shows if the user has new mesages
	char username[32], bufferShort[64], bufferLong[512];
	DATA info;
	
	//Username
	bzero(username,32);
	n = read(newsockfd,username, 31);
	if (n < 0)
		error("ERROR reading from socket");
	
	username[strlen(username)-1]='\0';
	
	
	sprintf(bufferShort, "Welcome  %s!", username);
	n = write(newsockfd,bufferShort,strlen(bufferShort));
	if (n < 0)
		error("ERROR writing to socket");
	
	do
	{
		//printf("READING socket: %d\n", newsockfd);
		n = read(newsockfd,&option,sizeof(option));
		if (n < 0)
			error("ERROR reading from socket");
		
		//Convert from network to host byte order
		option = ntohl(option);

		switch ((int)option)
		{
			//User wants to send a new message
			case 1:
				//Sender
				//username is used as sender
				strcpy(info.sender, username);

				//printf("Here is the sender: %s\n", username);

				//Receiver
				bzero(info.receiver,32);
				n = read(newsockfd,info.receiver,31);
				if (n < 0)
					error("ERROR reading from socket");
				
				info.receiver[strlen(info.receiver)-1]='\0';

				//printf("Here is the receiver: %s\n", info.receiver);

				//Message
				bzero(info.text,256);
				n = read(newsockfd,info.text,255);			
				if (n < 0)
					error("ERROR reading from socket");
				
				info.text[strlen(info.text)-1]='\0';

				//printf("Here is the message: %s\n", info.text);

				if (n < 0)
					error("ERROR writing to socket");

				insert(&head, info);
				
				//display(&head); //used for error checking
				break;
			
			//User wants to check for new messages
			case 2:
				found = 0;
				if (head->next)
				{
					temp = head;		
					while (temp->next)
					{
						temp = temp->next;
						
						if(strcmp(((DATA*)temp)->receiver, username) == 0)
						{
							//messageDisplay(&temp->data); //used for error checking
							
							//Sender
							sprintf(bufferShort, "NEW MESSAGE\nSENT FROM:%s\n", ((DATA*)temp)->sender);
							n = write(newsockfd,bufferShort,strlen(bufferShort));
							if (n < 0)
								error("ERROR writing to socket");
							usleep(100);
							
							//Text
							sprintf(bufferLong, "TEXT: %s\n", ((DATA*)temp)->text);
							n = write(newsockfd,bufferLong,strlen(bufferLong));
							if (n < 0)
								error("ERROR writing to socket");
							usleep(100);
							
							delete(&head, temp->data);
							
							found = 1;
						}
					}
				}

				if (!found)
				{
					n = write(newsockfd,"You have no new messages\n",26);
					if (n < 0)
						error("ERROR writing to socket");
				}
				
				//Ending signal,there are no more messages for the user
				sprintf(bufferShort, "done");
				n = write(newsockfd,bufferShort,strlen(bufferShort));
				if (n < 0)
					error("ERROR writing to socket");
				
				break;
				
			//User wants to close the application (exit)
			case 3:
				//printf("Closing connection %d\n", newsockfd);
				//usleep(100);
				pthread_exit(NULL);
				close(newsockfd);
				break;
				
			/*			
			case -1:
				//printf("Closing connection %d\n", newsockfd);
				//usleep(100);
				pthread_exit(NULL);
				close(newsockfd);
				break;*/
				
			default:
				break;

		}
		option = -1;
		
	}while(newsockfd >= 0);
	
	//printf("Closing connection %d\n", newsockfd);
	pthread_exit(NULL);
	close(newsockfd);
}
