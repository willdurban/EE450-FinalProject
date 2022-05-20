# EE450 Project ALI-COIN

by William Durban

Student ID: ###########
## Introduction

Below I have created a simulated blockchain called alicoin. 
Alicoin functions through utalizing client programs to interact with a main server that is able to recieve control a blockchain database located on three backend servers.
This is my first ever coding project and I am really proud of it! I hope you enjoy my project! 

## What I Have Done
I have completed all mandatory portions of the assignment (Phase 1, Phase 2, and Phase 3).
I have not comleted the optional phase 4.
In order to compile and begin exploring my code, compile using "$make all" in a terminal command line.



## Code Files
### Main Server:
C++ source file: serverM.cpp
compiled executable: serverM
To run: $./serverM
Description: Runs the main server that recieves all communication from clients and backend servers, does subsequestion calculation, and returns communication to clients and backend servers.

### Backend Servers:

#### Backend Server A:
C++ source file: serverA.cpp
compiled executable: serverA
To run: $./serverA
Description: Maintains record of block 1 of the alicoin blockchain.

#### Backend Server B:
C++ source file: serverB.cpp
compiled executable: serverB
To run: $./serverB
Description: Maintains record of block 2 of the alicoin blockchain.

#### Backend Server C:
C++ source file: serverC.cpp
compiled executable: serverC
To run: $./serverC
Description: Maintains record of block 3 of the alicoin blockchain.

### Clients:

#### Client A:
C++ source file: clientA.cpp
compiled executable: clientA
Commands to run: 
##### Check Balance:
$./clientA <username1>
#####Make a transaction:
$./clientA <username1> <username2> <amount>
##### Create Sorted Transaction List:
$./clientA TXLIST

Descriptions: Checks balance of a user, makes transactions between users, and 
creates a sorted transaction list (alichain.txt).

#### Client B:

C++ source file: clientB.cpp
compiled executable: clientB
Commands to run: 
#####Check Balance:
$./clientB <username1>
#####Make a transaction:
$./clientB <username1> <username2> <amount>
#####Create Sorted Transaction List:
$./clientB TXLIST

Descriptions: Checks balance of a user, makes transactions between users, and creates a sorted transaction list (alichain.txt).

### Makefile:
Source file: Makefile
To run:make all
Description: compiles all code into executables.

## Format of Messages: 

### Clients

#### Booting Up: 
“The client <A/B> is up and running.”

#### Upon sending the input to main server for checking balance:
"<USERNAME> sent a balance enquiry request to the
main server.”

#### Upon sending the input(s) to the main server for making a transaction.
“<SENDER_USERNAME> has requested to transfer <TRANSFER_AMOUNT> coins to <RECEIVER_USERNAME>.”

#### After receiving the balance information from the main server
“The current balance of <USERNAME> is : <BALANCE_AMOUNT> alicoins.”

#### After receiving the transaction information from the main server (if successful)
“<SENDER_USERNAME> successfully transferred <TRANSFER_AMOUT> alicoins to <RECEIVER_USERNAME>.
The current balance of <SENDER_USERNAME> is : <BALANCE_AMOUNT> alicoins.

#### After receiving the transaction information from the main server (if one of the clients is not part of the network)
“Unable to proceed with the transaction as <SENDER_USERNAME/RECEIVER_USERNAME> is not part of the network.”

#### After receiving the transaction information from the main server (if both the clients are not part of the network)
“Unable to proceed with the transaction as <SENDER_USERNAME> and <RECEIVER_USERNAME> are not part of the network.”

#### Upon sending the input to main server for requesting a sorted list
“Client <A/B> sent a sorted list request to the main server.”

### Main Server

#### Booting Up (only while starting): 
“The main server is up and running.”

#### Upon Receiving the username from the clients for checking balance:
“The main server received input=<USERNAME> from the client using TCP over port <port number>.”

#### Upon Receiving the username from the clients for transferring coins:
“The main server received from <SENDER_USERNAME> to transfer <TRANSFER_AMOUNT> coins to <RECEIVER_USERNAME> using TCP over port <port number>.”

#### After querying Backend-Server i for checking balance ( i is one of A,B, or C):
“The main server sent a request to server <i>.”

#### After receiving result from backend server i for checking balance ( i is one of A,B, or C):
“The main server received transactions from Server <i> using UDP over port <PORT_NUMBER>.”

#### After querying Backend-Server i for transferring coins ( i is one of A,B, or C):
“The main server sent a request to server <i>.”

#### After receiving result from backend server i for transferring coins (i is one of A,B, or C):
“The main server received the feedback from server <i> using UDP over por <PORT_NUMBER>.”

#### After sending the current balance to the client j (j is either A or B):
“The main server sent the current balance to client <j>.”

#### After sending the result of transaction to the client j (j is either A or B):
“The main server sent the result of the transaction to client <j>.”

### Backend Servers

#### Booting Up (Only while starting): 
“The Server<A/B/C> is up and running using UDP on port <port number>.”

#### Upon Receiving the request from main server
“The Server<A/B/C> received a request from the Main Server.”

#### After sending the results to the main server:
“The Server<A/B/C> finished sending the response to the Main Server.”

## Idiosyncrasy

### Insufficent Funds
This project does not have an error checking method if a user has insuffient funds for a transaction or if a user has negative balance.

### Given Clients called back to back
The main server is not able to handle requests from clientA back to back or clientB back to back. The clients need to be called this order clientA,clientB,clientA,clientB,clientA......

This also can be described as clients need to be called in alternating order with clientA starting first.
### Random Segmentation Fault
If I send a lot of client requests very quickly there seems to be a segmentation fault that happens. I assume this is a memory issue or an issue with my copy and pasting, but I thought I would document it just incase. 

### Serial Num in TXLIST
I believe I fixed this error, but sometimes randomly a serialNum transaction char string will be pasted on line 7 of my alichain.txt. I believe it is resolved as I think it was a memory issue, but it only happens randomly and I can't get it to happen again.

### Random Backend servers not so random
For some reason sending transactions to my backend servers seems to be less than random with severB and block2.txt receiving signifgantly more transactions than block1 and block2 when posting a transaction. I am not sure why this is but I assume it is due to the seeding of the random function as implenting random numbers is very simple and I believe I have done it correctly.

## Reused Code
For each server and client, creating the socket's code had inspiration from Beej's Socket Guide and EE250 example TCP and UDP servers.

To write lines I used this stack overflow post https://stackoverflow.com/questions/33979451/writing-lines-to-txt-file-c

For creating files of a given size i used stack overflow post. https://stackoverflow.com/questions/478075/creating-files-in-c

For replacing specific lines of a file I used source code from this youtube video https://www.youtube.com/watch?v=JKR0S84xLJ4

For reading lines from the block text files i used this geeks for geeks article about get_line https://www.geeksforgeeks.org/getline-string-c/ and stack overflow post https://stackoverflow.com/questions/7868936/read-file-line-by-line-using-ifstream-in-c

## Conclusion

This project was super interesting and absolutely tested my ability and personal resolve. This is my first ever coding project. I have not done a real workable project ever. The closest I have ever gotten was simple javascript games in highschool. I am honestly really proud of this and never thought I could be able to complete all three phases. 

Thank you for such a great and challanging project. I would be lying if there were moments were I was upset at the difficulty of the assignment given my limited experience with coding. 
