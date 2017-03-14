#include "defs.h"

int sockfd; //client socket descriptor

void signalHandler(int sig)
{
	char sendBuffer[MAXLINE];
	// printf("\nCtrl+C - Terminating Gracefully!! Don't force me :p..");
	sprintf(sendBuffer,"----||||----");
	send(sockfd,sendBuffer,MAXLINE,0);
	close(sockfd);
	exit(0);
}

void inputFormat(void)
{
	printf("\nInput format:<request type><item code><quantity>\n\nExample:\n\n010223 (for 23 units of item 102)\n\tor\n11111 (for close)\n============================================================\n");
}

int commandHandler(int sockfd)
{
	char sendBuffer[MAXLINE],recvBuffer[MAXLINE];
	inputFormat();

	while(1)
	{
		memset(sendBuffer,0,MAXLINE);
		memset(recvBuffer,0,MAXLINE);
		gets(sendBuffer);
		send(sockfd,sendBuffer,MAXLINE,0);
		recv(sockfd,recvBuffer,MAXLINE,0);
		puts(recvBuffer);

	}
}

int main(int argc, char *argv[])
{
	int check;
	int SERVER_PORT = atoi(argv[2]);
	struct sockaddr_in ServAddr;
	//Creating the socket
	sockfd=socket(AF_INET,SOCK_STREAM,0); //socket(internet_family,socket_type,protocol_value) retruns socket descriptor
	if(sockfd<0)
	{
		perror("Cannot create socket!");
		return 0;
	}

	//bzero(&ServAddr,sizeof(ServAddr)); //writes n no. of null nbytes to specified location

	//initializing the server socket
	ServAddr.sin_family=AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(argv[1]); //using the imput IP
	ServAddr.sin_port = htons(SERVER_PORT); //self defined server port

	if((connect(sockfd,(struct sockaddr *) &ServAddr,sizeof(ServAddr)))<0)
	{
		perror("Server is down!");
		return 0;
	}

	signal(SIGINT,signalHandler);

	printf("\n Connection established :\n ");

	while(1)
	{

		check=commandHandler(sockfd);
		if(check<0)
		{
			printf("Client closing socket!..exiting!\n");
			close(sockfd);
			exit(0);
		}
		else if(check==1)
		{
			close(sockfd);
			exit(0);
		}
	}

	return 0;
}
