#include"defs.h"
database *data; //Item-Name-Price list.
int records; //No. of Items in the database.
int listensd,connSD; //Socket descriptors.

void databaseInit(char *file_name)
{
	FILE *fp;
	char line_buff[MAXLINE];
	int i=0;
	fp=fopen(file_name,"r");
	if(fp==NULL)
	{
		// printf("\nDatabase Configuration Error.. Terminating Server.%s",file_name);
		signalHandler(0);
		exit(0);
	}
	fgets(line_buff,MAXLINE,fp); //reads the first line that has the number of entries in the text file
	records=atoi(line_buff); //gets number of records
	data=(database *)malloc(records*sizeof(database)); //allocates memory for database
	for(i=0;i<records;i++)
	{
		fscanf(fp,"%d",&data[i].itemUPC);
		fscanf(fp,"%s",data[i].item_name);
		fscanf(fp,"%lf",&data[i].price);
	}
	// printf("Database Configuration..... Complete\n" );
	return ;
}

int findUPC(int itemUPC)
{
	int i;
	for(i=0;i<records;i++) if(itemUPC==data[i].itemUPC) return i;
	return(-1);
}

void signalHandler(int sig)
{
	char msg[MAXLINE];
	close(listensd);
	// printf("Server terminating!..\n");
	sprintf(msg,"Server terminated!\n");
	send(connSD,msg,MAXLINE,0);
	close(connSD);
	exit(0);
}

void childprocess(int connSD,int id)
{
	int len,tokenCount,requestType,quantity,itemUPC,index,a;
	double total=0.0;
	char buffer[MAXLINE],msg[MAXLINE],*token;
	const char delim[2]=" ";
	while(1)
	{
		len=tokenCount=index=quantity=0;
		memset(msg,0,MAXLINE); //clears contents of msg
		len=recv(connSD,buffer,MAXLINE,0);
		if(len<0)
		{
			sprintf(msg,"Communication Error. Terminating Connection. Please reconnect.\n");
			send(connSD,msg,MAXLINE,0);
			close(connSD);
			exit(0);
		}
		if(strcmp(buffer,"----||||----")==0)
		{
			// printf("\nTerminating Child No: %d 'cause corresponding client terminated\n", getpid());
			close(connSD);
			exit(0);
		}
		token=strtok(buffer,delim);
		if(strlen(token)<5){
			sprintf(msg,"1$Protocol Error\n");
			send(connSD,msg,MAXLINE,0);
			continue;
		}
		requestType=token[0]-'0';
		itemUPC=(token[1]-'0')*100+(int)(token[2]-'0')*10+(int)(token[3]-'0');
		for(a=0;a<strlen(token)-4;a++)
		{
			quantity+=((int)token[a+4]-'0')*(int)pow(10.0,strlen(token)-a-5);
		}
		// printf("%d %d %d\n",requestType,itemUPC,quantity );
		if(requestType==0)
		{
			index=findUPC(itemUPC);
			if(index>=0)
			{
				total=total+(data[index].price * (float)quantity);
				sprintf(msg,"0$%.1f$%s\n",data[index].price,data[index].item_name);
				send(connSD,msg,MAXLINE,0);
				continue;
			}
			else
			{
				sprintf(msg,"1$UPC is not found in database\n");
				send(connSD,msg,MAXLINE,0);
				continue;
			}
		}
		else if(requestType==1)
		{
			sprintf(msg,"0$%.1f\n",total);
			send(connSD,msg,MAXLINE,0);
			close(connSD);
			// printf("\nClient completed the transaction. Terminating Child No: %d. \n", getpid());
			exit(0);
		}
		else
		{
			sprintf(msg,"1$Protocol Error\n");
			send(connSD,msg,MAXLINE,0);
			continue;
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
		if((connSD=accept(listensd,(struct sockaddr *)&CliAddr,&clilen))<0)
		{
			perror("Cannot establish connection!");
			return 0;
		}

		if((fork())==0)
		{
			close(listensd); //Allowing only one socket to listen by closing child process' listensd.
			// printf("\nRequest from %s servcied with child process %d\n",inet_ntoa(CliAddr.sin_addr) ,getpid());
			childprocess(connSD,getpid()); //A Child Process for a connection.
			close(connSD); //Child closes its version of connSD after transaction is completed.
			exit(0); //Child terminates.
		}
		close(connSD); //Parent looks for more connections by closing the connSD.
	}
}
