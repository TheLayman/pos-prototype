#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<string.h>
#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>
#include <math.h>
#define MAXLINE 100

void signalHandler(int sig); //programmer-defined signal handler for Ctrl+C command
void databaseInit(char *); //creates database
int findUPC(int); //searches and returns item_code supplied in the database and returns accordingly
void childprocess(int,int);
int commandHandler(int); //client places request with this command
void inputFormat(void);

typedef struct _data
{
	int itemUPC;
	char item_name[MAXLINE];
	double price;
}database;
