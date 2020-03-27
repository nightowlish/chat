#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

struct sockaddr_in serv; //This is our main socket variable.
int fd; //This is the socket file descriptor that will be used to identify the socket
int conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[1024] = ""; //This array will store the messages that are sent by the server
int seq_number = 0;
char username[16];
uint16_t user_len;

void *receiving(void *sockID)
{
	int socket = *((int *) sockID);

	while(1)
	{
		char data[1024];
		int read = recv(socket, data, 1024, 0);
		data[read] = '\0';
		if(strlen(data) != 0)
		{
			uint16_t us_len = data[0];
			char username[16];
			strncpy(username, data + 1, us_len);
			//username[1 + us_len] = '\0';
			uint16_t message_len = data[us_len + 1];

			int last_seq_num = seq_number;	
			
			seq_number = (int)data[us_len + 3];
	
			if(seq_number != last_seq_num + 1)
			{
				printf("Packet loss\n");
			}

			if(data[us_len + 2] == 'm')
				printf("%s: %s\n", username, data + 4 + us_len);
			if(data[us_len + 2] == 'a')
			{
				uint16_t flag = data[us_len + 4];
	
				if(flag == 1);
			}
			if(data[us_len + 2] == 'e')
			{
				uint16_t flag = data[us_len + 4];
	
				if(flag == 1)
				{
					close(fd);
					exit(0);
				}
			}
			memset(username, 0, sizeof username);
		}
		memset(data, 0, sizeof data);
	}
}

int main()
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8096);
	inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr); //This binds the client to localhost
	connect(fd, (struct sockaddr *)&serv, sizeof(serv)); //This connects the client to the server.

	pthread_t thread;
	pthread_create(&thread, NULL, receiving, (void *) &fd);
	
	while(1) 
	{
		char data[1024];
		//uint16_t seq_num = (uint16_t) seq_number;

    		//printf("Enter a message: ");
    		fgets(message, 1004, stdin);

		char command[7];
		strncpy(command, message, 7);

		uint16_t data_len = strlen(message);
		data_len = data_len - 1; // for \n

		if(strncmp(message, "connect", 7) == 0)
		{
			strncpy(username, message + 8, 16);
			int us_len = strlen(username);
			user_len = (uint16_t) us_len - 1;
		
			memcpy(data, &user_len, 1);
			memcpy(data + 1, username, user_len);

			data[user_len + 1] = data_len;
		
			data[2 + user_len] = 'c';
				
			seq_number++;
			uint16_t seq_num = (uint16_t) seq_number;			

			memcpy(data + 3 + user_len, &seq_num, 1);

			memcpy(data + 4 + user_len, message, data_len);
			data[user_len + data_len + 5] = '\0';			
		
			send(fd, data, strlen(data), 0);
			memset(data, 0, sizeof data);	
		}
		else if(strncmp(message, "exit", 4) == 0)
		{
			memcpy(data, &user_len, 1);
			memcpy(data + 1, username, user_len);

			memcpy(data + 1 + user_len, &data_len, 1);
		
			data[2 + user_len] = 'e';
				
			seq_number++;
			uint16_t seq_num = (uint16_t) seq_number;	

			memcpy(data + 3 + user_len, &seq_num, 1);

			memcpy(data + 4 + user_len, message, data_len);
			data[user_len + data_len + 5] = '\0';

			send(fd, data, strlen(data), 0);
			memset(data, 0, sizeof data);	
		}
		else
		{
			memcpy(data, &user_len, 1);
			memcpy(data + 1, username, user_len);

			data[user_len + 1] = data_len;
		
			data[2 + user_len] = 'm';
				
			seq_number++;
			uint16_t seq_num = (uint16_t) seq_number;	
	
			memcpy(data + 3 + user_len, &seq_num, 1);

			memcpy(data + 4 + user_len, message, data_len);
			data[user_len + data_len + 5] = '\0';

			send(fd, data, strlen(data), 0);
			memset(data, 0, sizeof data);	
			
			//An extra breaking condition can be added here (to terminate the while loop)
		}
		memset(message, 0, sizeof message);
	}

	close(fd);
}
