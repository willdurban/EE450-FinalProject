//DONE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>

using namespace std;
#define SERVER_M_PORT 24739
#define PORT 21739
#define localHost "127.0.0.1"
#define MAX_LINE 2048

int udp_socket;
struct sockaddr_in server_udp_address, main_udp_address;


//Transaction Datastructure to store and organize transactions
struct transaction 
{
    int rank;
    char* sender;
    char* reciever;
    int amount;
};
//command datastructure to store and organize commmands
struct command 
{
    int rank;
    char* username1;
    char* username2;
    int amount;
};
//recieve transactions from messages and input them into the proper datastructure format
struct transaction recieve_transaction(char * msg){
	
	struct transaction transaction;
	if(strcmp(msg,"done") == 0){
		transaction.rank = -1;
		transaction.sender = (char*) "done";
		transaction.reciever = (char*) "";
		transaction.amount = 0;	
	}else{
		transaction.rank = atoi(strtok(msg," "));
		transaction.sender = strtok(NULL," ");
		transaction.reciever = strtok(NULL," ");
		transaction.amount = atoi(strtok(NULL," "));
	}
	//printf("sender: %s, reciever: %s,amount: %d \n", transaction.sender,transaction.reciever,transaction.amount);
	return transaction;
}
//recieve commands from messages and input them into the proper datastructure format
struct command recieve_command(char* msg){
    struct command cmnd;
            if(strcmp(msg,"serialNum") == 0){
                cmnd.username1 = msg;
                cmnd.username2 = NULL;
                cmnd.amount = 0;
                cmnd.rank =-1;
            }else if(strcmp(msg,"txlist") == 0){
                cmnd.username1 = msg;
                cmnd.username2 = NULL;
                cmnd.amount = 0;
                cmnd.rank =-1;
            }else{
                cmnd.username1 = strtok(msg," ");
                char* temp = strtok(NULL," ");
                if(temp != NULL){
                    cmnd.username2 = temp;
                    cmnd.amount = atoi(strtok(NULL," "));
                    cmnd.rank = atoi(strtok(NULL," "));
                }else{
				    cmnd.username2 = (char*)"checkBal";
                    cmnd.amount = 0;
                    cmnd.rank =-1;
			    }
            }
	return cmnd;
}
//create udp socket to communicate with main server. Inspiration for code comes from Beej's guide and EE250 UDP server and client example code.
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

    main_udp_address.sin_family = AF_INET;
    main_udp_address.sin_port = htons(SERVER_M_PORT);
    main_udp_address.sin_addr.s_addr =  inet_addr(localHost);
}
//Sends all of a given names transactions to main server
void send_to_main(const char *msg)
{
    
    if((sendto(
        udp_socket,
        (const char *) msg,
        strlen(msg),
        MSG_CONFIRM,
        (const struct sockaddr *) &main_udp_address,
        sizeof(main_udp_address)
    )
    ) < 0)
    {
        perror("Send failed");
		exit(EXIT_FAILURE);
    };
   
    //printf("sent %s \n",msg);
}


//search the block txt file for a given name and return if it is inside the block or not. Reading line inspiration from geeks for geeks article and stack overflow post from readme
int search_block(char* name){
    
    string line;
    ifstream block;
    block.open("block1.txt");
    string space_delimiter = " ";
    if(block.is_open()){
        while(block.good()){
            getline(block, line);
            if(strcmp(line.c_str(),"\n")>0){
            struct transaction trans_line = recieve_transaction((char*)line.c_str());
            if(strcmp(trans_line.sender,name) == 0 || strcmp(trans_line.reciever,name)==0){
                return true;
            }
            }
        }
        
    }
   
    return false;
}
//Sends all of a given names transactions to main server. Reading line inspiration from geeks for geeks article and stack overflow post from readm
void check_balance(struct command cmnd){
    
    string line;
    ifstream block;
    block.open("block1.txt");
    string space_delimiter = " ";
    if(block.is_open()){
        char bTxt;
        while(block.good()){
            getline(block, line);
            if(strcmp(line.c_str(),"\n")>0){
                if(line.find(cmnd.username1)!=-1){
                    send_to_main(line.c_str());
                }
            }
        }
    }
    send_to_main((char *)"done");
}
//returns the max serial number found on the block file. Reading line inspiration from geeks for geeks article and stack overflow post from readm
int find_max_serial()
{
    int max = 0;
    string line;
    ifstream block;
    block.open("block1.txt");
    string space_delimiter = " ";
    if(block.is_open()){
        while(block.good()){
            getline(block, line);
            if(strcmp(line.c_str(),"\n")>0){
                struct transaction trans_line = recieve_transaction((char*)line.c_str());
                if(trans_line.rank > max){
                    max = trans_line.rank;
                }
            }
        }
    }
   
    return max;
}


//Post a transaction to the block txt file, I used writing line stack overflow post from readme for inspiration.
void post_to_txt(struct command cmnd){
        char new_trans[MAX_LINE];
        memset(new_trans,0,sizeof new_trans);
        sprintf(new_trans,"%d %s %s %d", cmnd.rank, cmnd.username1,cmnd.username2,cmnd.amount);
        //printf("posting: %s\n",new_trans);
        string new_line = string(new_trans);
        
        ofstream foutput; 
        ifstream finput;
        finput.open ("block1.txt");
        foutput.open ("block1.txt",ios::app); 
        
        if(finput.is_open()){
            foutput << new_line << "\n"; 
        }

        
        finput.close();
        foutput.close();
}
//send the entire block txt file to main server
void send_all_transactions(){
    string line;
    ifstream block;
    block.open("block1.txt");
    string space_delimiter = " ";
    if(block.is_open()){
        while(block.good()){
            getline(block, line);
            //printf("linething: %d",strcmp(line.c_str(),"\n"));
            if(strcmp(line.c_str(),"\n")>0){
                send_to_main(line.c_str());
                //printf("sending: %s \n",line.c_str());
            }else{
                //printf("sending: done");
                send_to_main("done");
                break;
            }
        }
        
    }
}
//Do all of the backend servers work. called in an infinite loop
void read_server(int udp_socket)
{
	char msg_buffer[MAX_LINE];
	int length, num;

	memset(msg_buffer,0,MAX_LINE);

	num = recvfrom(
	udp_socket,
	(char *) msg_buffer,
	MAX_LINE,
	0,
	(struct sockaddr *) &main_udp_address,
	(socklen_t *)sizeof main_udp_address
	);
    printf("The ServerA recieved a request from the Main Server \n");
	//printf("%s\n", msg_buffer );
    
	struct command cmnd = recieve_command(msg_buffer);
    
    //printf("%s %s \n", cmnd.username1,cmnd.username2);
    

    if(strcmp(cmnd.username1,"serialNum") == 0){
        int maxSerial = find_max_serial();
		char output[MAX_LINE];
		sprintf(output,"%d serialNum",maxSerial);
        send_to_main(output);
    }else if(strcmp(cmnd.username1,"txlist") == 0){
        send_all_transactions();
    }else if(!search_block(cmnd.username1)){
        send_to_main("username1");
    }else if(strcmp(cmnd.username2,"checkBal")==0){
        check_balance(cmnd);
    }else if(!search_block(cmnd.username2)){
        send_to_main("username2");
    }else if(!search_block(cmnd.username1)&&!search_block(cmnd.username2)){
        send_to_main("both");
    }
    if(cmnd.rank != -1){
        post_to_txt(cmnd);
    }

    printf("The ServerA finished sending the response to the Main Server \n");
}



int main(int argc, char const *argv[])
{
    create_udp_socket(udp_socket, PORT);
    printf("The ServerA is up and running using UDP on port %d \n", PORT);

        while(1){
        read_server(udp_socket);
    }
}