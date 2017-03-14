#include "defs.h"

int sockfd; //client socket descriptor

void signalHandler(int sig)
{
	char com_buff[MAXLINE];
	printf("\nCtrl+C - Terminating Gracefully!! Don't force me :p..");
	sprintf(com_buff,"-256");
	send(sockfd,com_buff,MAXLINE,0);
	close(sockfd);
	exit(0);
}

void inputFormat(void)
{
	printf("\nEnter command in the following format:\n<request type> <item code> <quantity>\n");
}

int commandHandler(int sockfd)
{
	char com_buff[MAXLINE],recv_buff[MAXLINE];
	inputFormat();

	while(1)
	{
		memset(com_buff,0,MAXLINE);
		memset(recv_buff,0,MAXLINE);
		gets(com_buff);
		send(sockfd,com_buff,MAXLINE,0);
		recv(sockfd,recv_buff,MAXLINE,0);
		if(recv_buff[2]=='i')
			printf("\n%s\n",recv_buff);

		if(recv_buff[0]=='T' || recv_buff[0]=='S')
		{
			printf("\n");
			fputs(recv_buff,stdout);
			close(sockfd);
			return 1;
		}

		if(recv_buff[2]=='o')
		{
			printf("\n");
			fputs(recv_buff,stdout);
			inputFormat();
			continue;
		}

		if(recv_buff[0]=='U')
		{
			printf("\n");
			fputs(recv_buff,stdout);
			continue;
		}
	}
signalHandler(1);
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
