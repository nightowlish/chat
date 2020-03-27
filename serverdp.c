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
//int seq_number = 1;

struct client{

	int index;
	int sockID;
	char username[16];
	int seq_number;

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

		uint16_t us_len = data[0];
		char command[7];
		strncpy(command, data + 4 + us_len, 7);

		char connect[7];
		strcpy(connect, "connect");
		//connect[8] = '\0';
		int ret = strcmp(command, connect);

		int last_seq_num = Client[index].seq_number;

		Client[index].seq_number = (int)data[3 + us_len];

		if(Client[index].seq_number != last_seq_num + 1)
		{
			printf("Packet loss!\n");

			char error_mess[100];
			memset(error_mess, 0, sizeof error_mess);

			strcpy(error_mess, "Your last message was not delivered! Please try again.\n");

			char response[1024];
			
			int usn_len = strlen(Client[index].username);
			uint16_t user_len= (uint16_t) usn_len;
	
			memcpy(response, &user_len, 1);
			memcpy(response + 1, Client[index].username, usn_len);
		
			int data_len = strlen(error_mess);
			uint16_t len = (uint16_t) data_len;
		
			memcpy(response + 1 + usn_len, &len, 1);
		
			//memcpy(response + 1 + usn_len -1 + 1, data, data_len);
			response[2 + usn_len] = 'm';
				
			Client[index].seq_number++;
			uint16_t seq_num = (uint16_t) Client[index].seq_number;
			memcpy(response + 3 + usn_len, &seq_num, 1);

			memcpy(response + 4 + usn_len, error_mess, strlen(error_mess));

			send(Client[index].sockID, response, strlen(response), 0);
			memset(response, 0, sizeof response);
			continue;
		}

		if(strcmp(command, connect) == 0)
		{
			char user[16];
			strncpy(user, data + 12 + us_len, 16);
			user[us_len] = '\0';
			printf("User: %s\n", user);
			strcpy(Client[index].username, user);

			char response[1024];
			
			int usn_len = strlen(user);
			uint16_t user_len= (uint16_t) usn_len;
	
			memcpy(response, &user_len, 1);
			memcpy(response + 1, Client[index].username, usn_len);
		
			int data_len = strlen(data);
			uint16_t len = 1;
		
			memcpy(response + 1 + usn_len, &len, 1);
		
			//memcpy(response + 1 + usn_len -1 + 1, data, data_len);
			response[2 + usn_len] = 'a';
				
			Client[index].seq_number++;
			uint16_t seq_num = (uint16_t) Client[index].seq_number;
			memcpy(response + 3 + usn_len, &seq_num, 1);
		
			uint16_t flag = 1;

			memcpy(response + 4 + usn_len, &flag, 1);

			send(Client[index].sockID, response, strlen(response), 0);
			memset(response, 0, sizeof response);
				
		}
		else if(strncmp(command, "exit", 4) == 0)
		{
			char response[1024];
			
			int usn_len = strlen(Client[index].username);
			uint16_t user_len= (uint16_t) usn_len;
	
			memcpy(response, &user_len, 1);
			memcpy(response + 1, Client[index].username, usn_len);
		
			int data_len = strlen(data);
			uint16_t len = 1;
		
			memcpy(response + 1 + usn_len, &len, 1);
		
			//memcpy(response + 1 + usn_len -1 + 1, data, data_len);
			response[2 + usn_len] = 'e';
				
			Client[index].seq_number++;
			uint16_t seq_num = (uint16_t) Client[index].seq_number;
			memcpy(response + 3 + usn_len, &seq_num, 1);
		
			uint16_t flag = 1;

			memcpy(response + 4 + usn_len, &flag, 1);

			send(Client[index].sockID, response, strlen(response), 0);
			memset(response, 0, sizeof response);
			close(Client[index].sockID);
			break;
		}
		else
		{
			for(int i = 0; i <= clientCount; i++) // send the message to all the other clients
			{
				char response[1024];
				
				int usn_len = strlen(Client[index].username);
				uint16_t user_len = (uint16_t) usn_len;

				
				memcpy(response, &user_len, 1);
				memcpy(response + 1, Client[index].username, usn_len);
		
				int data_len = strlen(data);
				uint16_t len = (uint16_t) data[1 + usn_len];
		
				memcpy(response + 1 + usn_len, &len, 1);
		
				//memcpy(response + 1 + usn_len -1 + 1, data, data_len);
				response[2 + usn_len] = 'm';
				
				Client[i].seq_number++;
				uint16_t seq_num = (uint16_t) Client[i].seq_number;
				memcpy(response + 3 + usn_len, &seq_num, 1);

				memcpy(response + 4 + usn_len, data + 4 + usn_len, data_len);
				
				//printf("Sending to clients: %s\n", response);
				
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
		Client[clientCount].seq_number = 0;

		pthread_create(&thread[clientCount], NULL, process_received_messages, (void *) &Client[clientCount]);
		
		clientCount++;
   
	}

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);

	close(fd);

}
