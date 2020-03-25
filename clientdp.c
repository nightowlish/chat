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

void *receiving(void *sockID) //functia prin care desface ce primeste de la server
{
	int socket = *((int *) sockID);

	while(1)
	{
		char data[1024];
		int read = recv(socket, data, 1024, 0); //while(1) e pentru functia asta
		//asteapta pana primeste de la server
		data[read] = '\0';
		if(strlen(data) != 0) //daca a primit ceva
		{
			uint16_t us_len = data[0]; //cate caractere are username-ul
			char username[16]; //pt a stii de la care dintre clienti a venit mesajul prin server
			strncpy(username, data + 1, us_len - 1);
			username[1 + us_len] = '\0';
			printf("%s: %s\n", username, data + 1 + us_len + 1);
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
    		//printf("Enter a message: ");
    		fgets(message, 1024, stdin); //ia mesajul clientului acesta
		//scanf("%s", message);

		/*char command[7];
		strncpy(command, message, 7);

		if(command == "connect")
		{
			char user[16];
			strncpy(user, message + 8, 16);
		}*/
    		send(fd, message, strlen(message), 0); //si il trimite la server
		//strcpy(message, "");
		//An extra breaking condition can be added here (to terminate the while loop)
	}

	close(fd);
}
