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

		for(int i = 0; i <= clientCount; i++) // send the message to all the other clients
		{
			if(Client[i].index != index)
			{
				//printf("Sending %s to client %i\n", data, index);
				send(Client[i].sockID, data, strlen(data), 0);

			}
		}
	}
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

}
