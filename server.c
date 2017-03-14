#include"defs.h"

database *data; //Item-Name-Price list.
int records; //No. of Items in the database.
int listensd,consd; //Socket descriptors.

void databaseInit(char *file_name)
{
	FILE *fp;
	char line_buff[MAXLINE];
	int ctr=0;
	fp=fopen(file_name,"r");
	if(fp==NULL)
	{
		printf("\nDatabase Configuration Error.. Terminating Server.%s",file_name);
		signalHandler(0);
		exit(0);
	}
	fgets(line_buff,MAXLINE,fp); //reads the first line that has the number of entries in the text file
	records=atoi(line_buff); //gets number of records
	data=(database *)malloc(records*sizeof(database)); //allocates memory for database
	for(ctr=0;ctr<records;ctr++)
	{
		fscanf(fp,"%d",&data[ctr].item_code);
		fscanf(fp,"%s",data[ctr].item_name);
		fscanf(fp,"%lf",&data[ctr].price);
	}
	printf("Database Configuration..... Complete\n" );
	return ;
}

int findUPC(int item_code)
{
	int ctr;

	for(ctr=0;ctr<records;ctr++)
	{
		if(item_code==data[ctr].item_code)
		{
			return(ctr);
		}
	}
	return(-1);
}

void signalHandler(int sig)
{
	char msg[MAXLINE];

	close(listensd);
	fputs("\nServer terminating!..",stdout);

	sprintf(msg,"Server terminated!\n");
	send(consd,msg,MAXLINE,0);

	exit(0);
}

void childprocess(int consd,int id)
{
	int len,token_ctr,request_type,quantity,item_code,index;
	double total=0.0;
	char buffer[MAXLINE],msg[MAXLINE],*token;
	const char delim[2]=" ";

	while(1)
	{
		len=token_ctr=index=quantity=0;

		memset(msg,0,MAXLINE); //clears contents of msg

		len=recv(consd,buffer,MAXLINE,0);
		if(len<0)
		{
			sprintf(msg,"3 : Error receiving command..Please reconnect.\n");
			send(consd,msg,MAXLINE,0);
			close(consd);
			exit(0);
		}
		printf("%s\n",buffer );

		if(strcmp(buffer,"-256")==0)
		{
			printf("\nTerminating Child No: %d 'cause corresponding client terminated\n", getpid());
			close(consd);
			exit(0);
		}

		token=strtok(buffer,delim);

		request_type=atoi(token); //tokenising request type from command

		while(token!=NULL)
		{
			token=strtok(NULL,delim);
			if(token_ctr==0)
				item_code=atoi(token);
			if(token_ctr==1)
				quantity=atoi(token);

			token_ctr++;
		}

		if(token_ctr<2)
		{
			sprintf(msg,"Protocol error..Please resend the request!\n");
			send(consd,msg,MAXLINE,0);
		}
		else
		{
			if(request_type==0)
			{
				index=findUPC(item_code);
				if(index>=0)
				{
					total=total+(data[index].price * quantity);

					sprintf(msg,"Price = %f\tItem name: %s\n",data[index].price,data[index].item_name);
					send(consd,msg,MAXLINE,0);
				}
				else
				{
					sprintf(msg,"UPC code error");
					send(consd,msg,MAXLINE,0);
				}
			}
			else if(request_type==1)
			{
				sprintf(msg,"Total cost = %f\n",total);
				send(consd,msg,MAXLINE,0);
				close(consd);
				printf("\nClient completed the transaction. Terminating Child No: %d. \n", getpid());
				exit(0);
			}
			else
			{
				sprintf(msg,"Protocol error..Please resend the request!\n");
				send(consd,msg,MAXLINE,0);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if(argc<2)
	{
		printf("\nToo few arguments to server!..exiting");
		exit(0);
	}
	int clilen; //clilen is the length of the client socket, used as a value-result argument
	struct sockaddr_in ServAddr, CliAddr; //sockaddr structure for sockets; one for server and the other for client
	listensd=socket(AF_INET,SOCK_STREAM,0); //Creating a TCP Socket
	if(listensd<0)
	{
		perror("Cannot create socket!");
		return 0;
	}
	signal(SIGINT,signalHandler);
	//Setting up the Server socket
	ServAddr.sin_family=AF_INET;
	ServAddr.sin_addr.s_addr = INADDR_ANY; //Connection from any IP.
	ServAddr.sin_port = htons(atoi(argv[1])); //self defined server port
	//Binding socket
	if(bind(listensd,(struct sockaddr *) &ServAddr, sizeof(ServAddr))<0)
	{
		perror("Cannot bind port!");
		return 0;
	}

	if (listen(listensd,SOMAXCONN) <0 )
	{
		perror("Unable to set the socket to listen");
		return 0;
	}
	//Initializing Database
	databaseInit("database.txt");
	while(1!=0)
	{
		clilen=sizeof(CliAddr);
		if((consd=accept(listensd,(struct sockaddr *)&CliAddr,&clilen))<0)
		{
			perror("Cannot establish connection!");
			return 0;
		}

		if((fork())==0)
		{
			close(listensd); //Allowing only one socket to listen by closing child process' listensd.
			printf("\nRequest from %s servcied with child process %d\n",inet_ntoa(CliAddr.sin_addr) ,getpid());
			childprocess(consd,getpid()); //A Child Process for a connection.
			close(consd); //Child closes its version of consd after transaction is completed.
			exit(0); //Child terminates.
		}
		close(consd); //Parent looks for more connections by closing the consd.
	}
}
