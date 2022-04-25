//DONE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
//#include <iostream>


using namespace std;
#define SERVER_PORT 25739
#define localHost "127.0.0.1"
#define MAX_LINE 2048

int tcp_socket;
struct sockaddr_in client_tcp_address, server_tcp_address;

//Create TCP socket. Code inspiration from Beej's guie and EE250 tcp client example
void client_tcp_socket()
{
	
	if((tcp_socket = socket(AF_INET,SOCK_STREAM,0)) < 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	memset((char *) &server_tcp_address, '\0', sizeof(server_tcp_address));
	memset(server_tcp_address.sin_zero, '\0', sizeof server_tcp_address.sin_zero);
	server_tcp_address.sin_family = AF_INET;
	server_tcp_address.sin_port = htons(SERVER_PORT);
	server_tcp_address.sin_addr.s_addr = inet_addr(localHost);
	

	if(connect(tcp_socket,(struct sockaddr *) &server_tcp_address, sizeof server_tcp_address)<0)
	{
		perror("connection failed");
		exit(EXIT_FAILURE);
	}
}
//Send a message to server. Code inspiration from Beej's guie and EE250 tcp client example
void send_to_server(const char* msg)
{
	if(send(tcp_socket, msg, strlen(msg), 0)<0){
		perror("Send failed");
		exit(EXIT_FAILURE);	
	}
}
//recieve a message from server. Code inspiration from Beej's guie and EE250 tcp client example
int recieve_server(){
	char msg_buffer[MAX_LINE];
	memset(msg_buffer,0,sizeof msg_buffer);
	

	int num = read(tcp_socket, msg_buffer ,sizeof msg_buffer);
	int balance = atoi(msg_buffer);
	return balance;
}


int main(int argc, char const *argv[])
{
	//Create the TCP socket to interface with Main Server 
	client_tcp_socket();
	printf("The client A is up and running \n");
	//Check if client wants a transaction or to sent a txtlist/checkbalance
	if(argc == 2)
	{
		if(strcmp(argv[1],"TXLIST")==0){
			printf("Client A sent a sorted list request to the main server. \n");
			send_to_server(argv[1]);
		}else{
			printf("<%s> sent a balance enquiry to the main server \n",argv[1]);
			send_to_server(argv[1]);
			int balance = recieve_server();
			if(balance == -1){
				printf("Unable to proceed with the transaction as <%s> is not a part of the network \n", argv[1]);
			}else{
				printf("The current balance of <%s> is <%d> alicoins. \n", argv[1], balance);
			}
		}
	}else if(argc == 4){
		char args[100] = "";
		memset(args,0,sizeof args);
		strcat(args, argv[1]);
		strcat(args," ");
		strcat(args, argv[2]);
		strcat(args," ");
		strcat(args, argv[3]);
		//printf("\nARGS: %s \n",args);
		send_to_server(args);
		int balance = recieve_server();
		//Work with if user is not in network and return check bal and transactions.
		printf("<%s> has requested to transfer <%d> coins to <%s>. \n",argv[1],atoi(argv[3]),argv[2]);
		if(balance == -1){
			printf("Unable to proceed with the transaction as <%s> is not a part of the network \n", argv[1]);
		}else if(balance == -2){
			printf("Unable to proceed with the transaction as <%s> is not a part of the network \n", argv[2]);
		}else if(balance == -3){
			printf("Unable to proceed with the transaction as <%s> and <%s> are not a part of the network \n", argv[1],argv[2]);
		}else{
			printf("<%s> sucessfully transferred <%d> alicoins to <%s>. \n", argv[1], atoi(argv[3]),argv[2]);
			
			printf("The current balance of <%s> is <%d> alicoins \n", argv[1], balance);
		}
		
		
		
	}
	//clear arguments just in case.
	for(int i = 0; i<argc; i++){
		memset((char*)argv[i],0,sizeof argv[i]);
	}
}
	
	
	
