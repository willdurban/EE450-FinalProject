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
#include <iostream>
#include <fstream>

using namespace std;
#define localHost "127.0.0.1"
#define SERVER_PORT_B 22739
#define SERVER_PORT_A 21739
#define SERVER_PORT_C 23739
#define UDP_PORT 24739
#define CLIENT_PORT_A 25739
#define CLIENT_PORT_B 26739
#define MAX_LINE 2048


struct transaction 
{
    int rank;
    char* sender;
    char* reciever;
    int amount;
};

struct command 
{
    char* username1;
    char* username2;
    int amount;
};

int tcp_socketA,tcp_socketB,udp_socket;
int arg_num;
struct sockaddr_in serverA_tcp_address,serverB_tcp_address, server_udp_address, client_tcp_address,back_A_udp_address,back_B_udp_address,back_C_udp_address;


//Create TCP socket, Code Inspriation from Beej Programing guide and EE250 TCP server example
void create_tcp_socket(int &tcp_socket,int port)
{
	if((tcp_socket = socket(AF_INET,SOCK_STREAM,0)) == 0){
		perror("tcp socket failed");
		exit(EXIT_FAILURE);
	}
	int opt = 1;
		if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		&opt, sizeof(opt)))
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
	if(port == CLIENT_PORT_A){
		memset((char *) &serverA_tcp_address, '\0', sizeof(serverA_tcp_address));
		serverA_tcp_address.sin_family = AF_INET;
		serverA_tcp_address.sin_port = htons(port);
		serverA_tcp_address.sin_addr.s_addr = inet_addr(localHost);
		memset(serverA_tcp_address.sin_zero, '\0', sizeof serverA_tcp_address.sin_zero);

		if((bind(tcp_socket, (struct sockaddr *)&serverA_tcp_address, sizeof serverA_tcp_address)) < 0 ){
			perror("bind failed");
			exit(EXIT_FAILURE);
		}

		if(listen(tcp_socket,4)){
			perror("Listen failed");
			exit(EXIT_FAILURE);
	}
	}else if(port == CLIENT_PORT_B){
		memset((char *) &serverB_tcp_address, '\0', sizeof(serverB_tcp_address));
		serverB_tcp_address.sin_family = AF_INET;
		serverB_tcp_address.sin_port = htons(port);
		serverB_tcp_address.sin_addr.s_addr = inet_addr(localHost);
		memset(serverB_tcp_address.sin_zero, '\0', sizeof serverB_tcp_address.sin_zero);
		
		if((bind(tcp_socket, (struct sockaddr *)&serverB_tcp_address, sizeof serverB_tcp_address)) < 0 ){
			perror("bind failed");
			exit(EXIT_FAILURE);
		}

		if(listen(tcp_socket,4)){
			perror("Listen failed");
			exit(EXIT_FAILURE);
		}
	}
}

//Configure all backend IP addresses,Code Inspriation from Beej Programing guide and EE250 TCP server example
void setup_backend_addresses(){

	back_A_udp_address.sin_family = AF_INET;
    back_A_udp_address.sin_port = htons(SERVER_PORT_A);
    back_A_udp_address.sin_addr.s_addr = inet_addr(localHost);
	

	back_B_udp_address.sin_family = AF_INET;
    back_B_udp_address.sin_port = htons(SERVER_PORT_B);
    back_B_udp_address.sin_addr.s_addr = inet_addr(localHost);
	

	back_C_udp_address.sin_family = AF_INET;
    back_C_udp_address.sin_port = htons(SERVER_PORT_C);
    back_C_udp_address.sin_addr.s_addr = inet_addr(localHost);
	
}

//Create UDP socket to talk to backend.Code Inspiration from Beej Programmign guie and EE250 UDP server example
void create_udp_socket(int &udp_socket, int port)
{
	if((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		
		perror("udp socket failed");
		exit(EXIT_FAILURE);
	}
		
	memset((char *) &server_udp_address, '\0', sizeof(server_udp_address));
	server_udp_address.sin_family = AF_INET;
	server_udp_address.sin_port = htons(port);
	server_udp_address.sin_addr.s_addr = inet_addr(localHost);
	memset(server_udp_address.sin_zero, '\0', sizeof server_udp_address.sin_zero);

	if((bind(udp_socket, (struct sockaddr *)&server_udp_address, sizeof server_udp_address)) < 0 ){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	setup_backend_addresses();
}


//Command Datastructure to organize and store commands from client
struct command recieve_command(char* msg){
    struct command cmnd;
			if(strcmp(msg,"TXLIST") == 0){
				cmnd.username1 = (char*)"txlist";
				cmnd.username2 = NULL;
				cmnd.amount = 0; 
			}else{
				cmnd.username1 = strtok(msg," ");
				char* temp = strtok(NULL," ");
				if(temp != NULL){
					cmnd.username2 = temp;
					cmnd.amount = atoi(strtok(NULL," "));
				}else{
					cmnd.username2 = (char*)"checkBal";
				}
			}
	return cmnd;
}

//Sends a input (msg) to a backend server
void send_to_backend(char *msg,struct sockaddr_in address)
{
    //printf("%d \n", htons(address.sin_port));
    if((sendto(
        udp_socket,
        (const char *) msg,
        strlen(msg),
        MSG_CONFIRM,
        (const struct sockaddr *) &address,
        sizeof(address)
    )
    ) < 0)
    {
        perror("Send failed");
		exit(EXIT_FAILURE);
    };
	if(address.sin_port == back_A_udp_address.sin_port){
    	printf("The main server sent a request to Server <A> \n");
	}else if(address.sin_port == back_B_udp_address.sin_port){
		 printf("The main server sent a request to Server <B> \n");
	}else{
		 printf("The main server sent a request to Server <C> \n");
	}
}

//returns a transaction from a message
struct transaction recieve_transaction(char * msg){
	struct transaction transaction;
	if(strcmp(msg,"done") == 0)
	{
		transaction.rank = -1;
		transaction.sender = (char*) "done";
		transaction.reciever = (char*) "";
		transaction.amount = 0;	
	}else if(string(msg).find("serialNum") != -1)
	{
		transaction.rank = atoi(strtok(msg," "));
		transaction.sender = strtok(NULL," ");
		transaction.reciever = NULL;
		transaction.amount = 0;
	}
	else if(strcmp(msg,"username1") == 0 || (strcmp(msg,"username2")) == 0 || (strcmp(msg,"both") == 0))
	{
		transaction.rank = -1;
		transaction.sender = msg;
		transaction.reciever = (char*) "";
		transaction.amount = 0;	
	}
	else
	{
		transaction.rank = atoi(strtok(msg," "));
		transaction.sender = strtok(NULL," ");
		transaction.reciever = strtok(NULL," ");
		transaction.amount = atoi(strtok(NULL," "));

	}
	//printf("sender: %s, reciever: %s,amount: %d \n", transaction.sender,transaction.reciever,transaction.amount);
	return transaction;
}


//Returns true if backend server has name, returns false if username1, username2, or both are not found. 
int msg_checker(char* msg){
	
	if(string(msg).find("both") == -1 && string(msg).find("username1") == -1 && string(msg).find("username2") ==-1){
		return true;
	}else{
		return false;
	} 
}


//reads the UDP socket for a specifc backend server, returns a transaction
struct transaction read_server(struct sockaddr_in back_udp_address)
{
	
	char msg_buffer[MAX_LINE] = "";
	memset(msg_buffer,0,sizeof msg_buffer);
	int length, num;

	

	num = recvfrom(
	udp_socket,
	msg_buffer,
	MAX_LINE,
	0,
	(struct sockaddr *) &back_udp_address,
	(socklen_t *)sizeof back_udp_address
	);
	//printf("from server: %s\n", msg_buffer);

	struct transaction line = recieve_transaction(msg_buffer);

	return line;
}
//checks if a user is invalid or not from data recieved from back end servers.
int inv_user(char* msg){
	if(strcmp(msg,"username1") == 0 || strcmp(msg,"username2") == 0 || strcmp(msg,"both") == 0){
		return true;
	}else{
		return false;
	}
}
//computs the balance of a given user and returns it.
int check_balance(char* user, int port){
		struct transaction trans_line;
		int numInvalid = 0;
		int userSent = 0;
		int userRecv = 0;
		int userBalance = 1000;
		send_to_backend(user,back_A_udp_address);
		
		trans_line = read_server(back_A_udp_address);

		if(!inv_user(trans_line.sender)){
			while(strcmp(trans_line.sender,"done") != 0){
				userSent = 0;
				userRecv = 0;
				
				if(strcmp(trans_line.sender,user) == 0){
					userSent = userSent + trans_line.amount;
					//printf("Sent: %d \n", userSent);
				}
				if(strcmp(trans_line.reciever, user) == 0){
					userRecv = userRecv + trans_line.amount;
					//printf("Recv: %d \n", userRecv);
				}
				userBalance = userBalance - userSent + userRecv;
				
				trans_line = read_server(back_A_udp_address);
			
			}
			printf("The main server received transactions from Server <A> using UDP over port %d \n",htons(back_A_udp_address.sin_port));
		}else{
			numInvalid++;
		}
	
		send_to_backend(user,back_B_udp_address);

		trans_line = read_server(back_B_udp_address);

		if(!inv_user(trans_line.sender)){
			while(strcmp(trans_line.sender,"done") != 0){
				userSent = 0;
				userRecv = 0;
				
				if(strcmp(trans_line.sender,user) == 0){
					userSent = userSent + trans_line.amount;
					//printf("Sent: %d \n", userSent);
				}
				if(strcmp(trans_line.reciever, user) == 0){
					userRecv = userRecv + trans_line.amount;
					//printf("Recv: %d \n", userRecv);
				}
				userBalance = userBalance - userSent + userRecv;
				
				trans_line = read_server(back_B_udp_address);
				
			}
			printf("The main server received transactions from Server <B> using UDP over port %d \n",htons(back_B_udp_address.sin_port));
		
		}else{
			numInvalid++;
		}
	
		
		send_to_backend(user,back_C_udp_address);
		
		trans_line = read_server(back_C_udp_address);
		if(!inv_user(trans_line.sender)){
		while(strcmp(trans_line.sender,"done") != 0){
			userSent = 0;
			userRecv = 0;
			
			if(strcmp(trans_line.sender,user) == 0){
				userSent = userSent + trans_line.amount;
				//printf("Sent: %d \n", userSent);
			}
			if(strcmp(trans_line.reciever, user) == 0){
				userRecv = userRecv + trans_line.amount;
				//printf("Recv: %d \n", userRecv);
			}
			userBalance = userBalance - userSent + userRecv;
			
			trans_line = read_server(back_C_udp_address);
			printf("The main server received transactions from Server <C> using UDP over port %d \n",htons(back_C_udp_address.sin_port));
		
		}
		}else{
			numInvalid++;
		}

		if(numInvalid == 3){
			return -1;
		}
	return userBalance;
}
////Code Inspriation from Beej Programing guide and EE250 TCP server example

//sends a message to a given client.
void send_to_client(char *msg,int tcp_socket)
{
    
	if(send(tcp_socket, msg, strlen(msg), 0)<0){
		perror("Send failed");
		exit(EXIT_FAILURE);	
	}
	//printf("msg: %s \n", msg);

}

//finds the highest serial number present in the blockchain,
int next_serial_number(){
	int max = 0;
	send_to_backend((char*)"serialNum",back_A_udp_address);
	struct transaction serialA = read_server(back_A_udp_address);
	int maxA = serialA.rank;
	if(maxA > max){
		max = maxA;
	}
	send_to_backend((char*)"serialNum",back_B_udp_address);
	struct transaction serialB = read_server(back_B_udp_address);
	int maxB = serialB.rank;
		if(maxB > max){
		max = maxB;
		}
	send_to_backend((char*)"serialNum",back_C_udp_address);
	struct transaction serialC = read_server(back_C_udp_address);
	int maxC = serialC.rank;
		if(maxC > max){
		max = maxC;
		}

	//printf("max: %d \n", max);

	return max;
}

//replaces a given line in the alichain.txt TXLIST
void replace_line(char replace[MAX_LINE],int serial,int num_lines){
	FILE *file,*temp;
    file = fopen("alichain.txt","r");
    temp =fopen("temp.txt","w");

    bool keep_reading = true;

    // will keep track of the current line number we are reading from the file
    int current_line = 1;
    int replace_line = serial;
    char buffer[MAX_LINE];
	memset(buffer,0,sizeof buffer);

    while(current_line <= num_lines) 
    {
		
		fgets(buffer, MAX_LINE, file);
        //printf("\n Current Line: %d, Inputing: %d. buffer: %s \n", current_line,replace_line,buffer);
       
        if (current_line == replace_line){
        	fputs(replace, temp);
			if(current_line != num_lines)
			fputs(buffer,temp);
		}
        else fputs(buffer, temp);
        
		current_line++;
        
    } 
    

    fclose(file);
    fclose(temp);
  
  // delete the original file, rename temp file to the original file's name
    remove("alichain.txt");
    rename("temp.txt", "alichain.txt");
}
//creates the alichain.txt and temp.txt file
void create_files(int numLines){
	ofstream outfile ("alichain.txt");
    FILE *file, *temp;

    for(int i = 1;i < numLines;i++){
        outfile << "\n";
    }
	

    outfile.close();

    ofstream outfile2 ("temp.txt");
	for(int i = 1;i < numLines;i++){
        outfile2 << "\n";
    }
	
    outfile.close();

}
//gets all of the block data from a given given backend server, and saves it in alichain.txt
void get_block_lists(struct sockaddr_in back_udp_address, int num_lines){
		struct transaction trans_line;
		send_to_backend((char*)"txlist",back_udp_address);
		trans_line = read_server(back_udp_address);
		while(strcmp(trans_line.sender,"done") != 0){
			char blockLine[MAX_LINE] = "";
			memset(blockLine,0,sizeof blockLine);
			char* sender = trans_line.sender;

			sprintf(blockLine,"%d %s %s %d",trans_line.rank,trans_line.sender,trans_line.reciever,trans_line.amount);

			//printf("blockline: %s \n", blockLine);
			//printf("blockline serial: %d \n", trans_line.rank);
			replace_line(blockLine,trans_line.rank,num_lines);

			trans_line = read_server(back_A_udp_address);
		}
}
//reads input from a given client and does all the work for the main server. Is called in a infinite loop alternating between clients.
void read_client(int tcp_socket,int client_port)
{
	int userBalance = 0;
	char msg_buffer[MAX_LINE] = "";
	memset(msg_buffer,0,sizeof msg_buffer);
	int child_socket;
	socklen_t client_address_length = sizeof(client_tcp_address);

	if((child_socket = accept(tcp_socket, (struct sockaddr* )&client_tcp_address, &client_address_length)) < 0 ){
		perror("accept");
		exit(EXIT_FAILURE);
	}

	
	int num = recv(child_socket, msg_buffer ,sizeof msg_buffer,0);
	//printf("\n Rec MSG BUFFER: (%s) \n", msg_buffer);
	

	struct command comm = recieve_command(msg_buffer);

	if(strcmp(comm.username1,"txlist") == 0){
		int list_lines = next_serial_number();
		create_files(list_lines);
		
		get_block_lists(back_A_udp_address,list_lines);
		get_block_lists(back_B_udp_address,list_lines);
		get_block_lists(back_C_udp_address,list_lines);
	}
	else if(strcmp(comm.username2,"checkBal") == 0){
		printf("The main server received input=<%s> from the client using TCP over port %d \n", comm.username1,client_port);
		userBalance = check_balance(comm.username1,client_port);
		if(userBalance == -1){
			int errorUser1 = -1;
			char output[MAX_LINE];
			sprintf(output,"%d",errorUser1);
			send_to_client(output,child_socket);
		}else{
		char output[MAX_LINE];
		sprintf(output,"%d",userBalance);
		send_to_client(output, child_socket);
		}
		char* clientLetter;
		if(client_port == CLIENT_PORT_A){
			clientLetter = (char*)"A";
		}else{
			clientLetter = (char*)"B";
		}
	
		printf("The main server sent the current balance to client <%s> \n", clientLetter);
	}
	else
	{
		printf("The main server received from <%s> to transfer <%d> coins to <%s> using TCP over port <%d> \n",comm.username1,comm.amount,comm.username2,client_port);
		int user1Balance = check_balance(comm.username1,client_port);
		int user2Balance = check_balance(comm.username2,client_port);
		//printf("%d , %d \n", user1Balance,user2Balance);

		if(user1Balance == -1 && user2Balance != -1)
		{
			int errorUser1 = -1;
			char output[MAX_LINE];
			sprintf(output,"%d",errorUser1);
			send_to_client(output,child_socket);
		
		}else if(user2Balance == -1 && user1Balance != -1)
		{
			int errorUser2 = -2;
			char output[MAX_LINE];
			sprintf(output,"%d",errorUser2);
			send_to_client(output,child_socket);
		}
		else if(user2Balance == -1 && user1Balance == -1)
		{
			int errorBoth = -3;
			char output[MAX_LINE];
			sprintf(output,"%d",errorBoth);
			send_to_client(output,child_socket);
		}
		else
		{
			int serial = next_serial_number()+1;
			char* sender = comm.username1;
			char* reciever = comm.username2;
			int amount = comm.amount;
			char transaction[MAX_LINE];
			sprintf(transaction,"%s %s %d %d",sender,reciever,amount, serial);
			int serverNum = 0;
			serverNum = rand() % 3;
			if(serverNum == 0){
				send_to_backend(transaction,back_A_udp_address);
				printf("The main server received the feedback from server A using UDP over port <%d> \n",htons(back_A_udp_address.sin_port));
			}else if(serverNum == 1){
				send_to_backend(transaction,back_B_udp_address);
				printf("The main server received the feedback from server B using UDP over port <%d> \n",htons(back_B_udp_address.sin_port));
			}else if(serverNum == 2){
				send_to_backend(transaction,back_C_udp_address);
				printf("The main server received the feedback from server C using UDP over port <%d> \n",htons(back_C_udp_address.sin_port));
			}
			char output[MAX_LINE];
			sprintf(output,"%d",user1Balance-amount);
			send_to_client(output,child_socket);
			
			char* clientLetter;
			if(client_port == CLIENT_PORT_A){
				clientLetter = (char*)"A";
			}else{
				clientLetter = (char*)"B";
			}
			printf("The main server sent the result of the transaction to client <%s> \n",clientLetter);
		}
	}

	
	close(child_socket);
	
	
	
}


int main(int argc, char const *argv[])
{
	create_tcp_socket(tcp_socketA,CLIENT_PORT_A);
	create_tcp_socket(tcp_socketB,CLIENT_PORT_B);
	create_udp_socket(udp_socket,UDP_PORT);
	
	printf("The main server is up and running. \n");
	while(true){
	read_client(tcp_socketA,CLIENT_PORT_A);
	read_client(tcp_socketB,CLIENT_PORT_B);
	}
	return 0;
		
}
