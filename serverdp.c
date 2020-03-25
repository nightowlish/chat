#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

struct sockaddr_in serv; //This is our main socket variable.
int fd; //This is the socket file descriptor that will be used to identify the socket
int conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[100] = ""; //This array will store the messages that are sent by the server
int clientCount = 0;

struct client{

	int index;
	int sockID;
	char username[16];

};

struct client Client[1024];
pthread_t thread[1024];

void *process_received_messages(void *ClientDetail)
{
	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;

	while(1)
	{
		char data[1024];
		int read = recv(clientSocket,data,1024,0); // store the data sent from the client
		data[read] = '\0';
		printf("Server received: %s\n", data);

		char command[7];
		strncpy(command, data, 7);
		printf("Command: %s\n", command);

		char connect[7];
		strcpy(connect, "connect");
		//connect[8] = '\0';
		printf("Connect is: %s\n", connect);
		int ret = strcmp(command, connect);
		printf("Ret is: %i\n", ret);

		if(strcmp(command, connect) == 0)
		{
			printf("Matches with connect\n");
			char user[16];
			strncpy(user, data + 8, 16);
			printf("User: %s\n", user);
			strcpy(Client[index].username, user);
		}
		else if(strncmp(command, "exit", 4) == 0)
		{
			printf("Matches with exit\n");
			close(clientSocket);
			break;
		}
		else
		{
			printf("Just a message\n");
			for(int i = 0; i <= clientCount; i++) // send the message to all the other clients
			{
				//printf("Sending %s to client %i\n", data, index);
				char response[1024];
				
				int usn_len = strlen(Client[index].username);
				uint16_t user_len = (uint16_t) usn_len;
				/*if(usn_len >= 10)
					memcpy(response, &user_len, 2);
				else
					memcpy(response, &user_len, 1);*/

				
				memcpy(response, &user_len, 1);
				memcpy(response + 1, Client[index].username, usn_len - 1);
		
				int data_len = strlen(data);
				uint16_t len = (uint16_t) data_len;
		
				memcpy(response + 1 + usn_len - 1, &len, 1);
		
				memcpy(response + 1 + usn_len -1 + 1, data, data_len);
				//response[3 + usn_len + data_len + 1] = '\0';
				for(int i = 0; i < 20; i++)
					printf("%c is at %i\n", response[i], i);
				printf("Sending to clients: %s\n", response);
				
				send(Client[i].sockID, response, strlen(response), 0);
				memset(response, 0, sizeof response);
			}
		}		
	}
	return NULL;
}

int main()
{
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8096); //Define the port at which the server will listen for connections.
	serv.sin_addr.s_addr = INADDR_ANY;
	fd = socket(AF_INET, SOCK_STREAM, 0); //This will create a new socket and also return the identifier of the socket into fd.
	// To handle errors, you can add an if condition that checks whether fd is greater than 0. If it isn't, prompt an error
	bind(fd, (struct sockaddr *)&serv, sizeof(serv)); //assigns the address specified by serv to the socket
	listen(fd,1024); //Listen for client connections. Maximum 1024 connections will be permitted.
	//Now we start handling the connections.
	while(1) 
	{
		Client[clientCount].sockID = accept(fd, (struct sockaddr *)NULL, NULL);
		Client[clientCount].index = clientCount;

		pthread_create(&thread[clientCount], NULL, process_received_messages, (void *) &Client[clientCount]);
		
		clientCount++;
   
	}

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);

	close(fd);

}
