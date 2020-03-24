/*
#define LINE_SIZE 1024
#define DB_FILE "db.csv"

struct sockaddr_in serv; //This is our main socket variable.
int fd; //This is the socket file descriptor that will be used to identify the socket
int conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[1024] = ""; //This array will store the messages that are sent by the server
int clientCount = 0;*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 4444

int main(){

	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024];
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", 4444);

	if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		printf("[-]Error in binding.\n");
	}

	int clients[12] = {-1};
	int cnt = 0;
	
	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		//When having more than the fixed number of clients, the server will exit
		//the max number can be modified in the clients array above
		if(cnt<12) {
			clients[cnt++] = newSocket;
		}
		else {
			printf("Maximum number of clients reached...\n");
			exit(1);
		}

		if((childpid = fork()) == 0){
			close(sockfd);

			while(1){
				recv(newSocket, buffer, 1024, 0);
				if(strcmp(buffer, ":exit") == 0){
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}else{
					printf("Client: %s\n", buffer);
					//Send to all the clients the message received from the current client
					for(int i=0; i<cnt;i++) {
						send(clients[i], buffer, strlen(buffer), 0);
					}
					bzero(buffer, sizeof(buffer));
				}
			}
		}

	}

	close(newSocket);


	return 0;
}

/*struct client{

	int index;
	int sockID;
	char *username;
	int seq_number;

};

struct client Client[1024];
pthread_t thread[1024];

char* getFieldFromLine(char* line, int fieldIndex) {
    char* tokenPtr;
    for (tokenPtr = strtok(line, ","); tokenPtr && *tokenPtr; tokenPtr = strtok(NULL, ",\n")) {
        if (!--fieldIndex)
            return tokenPtr;
    }
    return NULL;
}

char* checkUsername(char* username, int socket, int i) {
	printf("Checking username on the server side...\n"); 
    int found = 0;
    char line[LINE_SIZE];
    FILE *db = fopen(DB_FILE, "r");
    
    while (fgets(line, LINE_SIZE, db)) {
        char* tmp = strdup(line); 

        char* usernameDB = getFieldFromLine(tmp, 1);

        if (strcmp(usernameDB, username) == 0) {
            found = 1;
            free(tmp);
            break;
        }

        free(tmp); // strtok clobbers temp
    }

    	fclose(db);

	char response[1024];
	uint16_t socket_num = (uint16_t) socket;
	uint16_t seq_num = (uint16_t) Client[i].seq_number;
	
	memcpy(response + 0, &socket_num, 1);
	response[1] = 'u';

	memcpy(response + 2, &seq_num, 1);

	printf("Found is: %i\n", found);	
	
    	if(found != 0)
	{
		response[3] = '1';
		response[4] = '\0';
		printf("Response for username check: %s\n", response);
		//write(socket, response, 1024);
		return response;
	}
    	else
	{
		response[3] = '2';
		response[4] = '\0';
		printf("Response for username checkt: %s\n", response);
		//write(socket, response, 1024);
		return response;
	}
}

int checkCredentials(char* username, char* password, int socket, int i) {
		printf("Credential checking done on the server side...\n");
    	int found = 0;
    	char line[LINE_SIZE];
    	FILE *db = fopen(DB_FILE, "r");

    	char stringPassword[16];
    	//sprintf(stringPassword, "%lu", password);
    
    	while (fgets(line, LINE_SIZE, db)) {
        	char* tmp = strdup(line); 
        	char* tmp2 = strdup(line);
        
        	char* usernameDB = getFieldFromLine(tmp, 1);
        	char* passwordDB = getFieldFromLine(tmp2, 2);

        	if (strcmp(usernameDB, username) == 0 && strcmp(passwordDB, password) == 0) {
            		found = 1;
            		free(tmp);
            		free(tmp2);
            		break;
        	}

        	// strtok clobbers temp
        	free(tmp); 
        	free(tmp2);
    	}

    	fclose(db);

	char response[1024];
	uint16_t socket_num = (uint16_t) socket;
	uint16_t seq_num = (uint16_t) Client[i].seq_number;
	
	memcpy(response + 0, &socket_num, 1);

	response[1] = 'c';

	memcpy(response + 2, &seq_num, 1);

	printf("Found is: %i\n", found);
	

  	if(found)
	{
		response[3] = '0';
		response[4] = '\0';
		write(socket, response, 1024);
        	return 0;
	}
    	else
	{
		response[3] = '1';
		response[4] = '\0';
		write(socket, response, 1024);
        	return -1;
	}
}

int registerCredentials(char* username, char* password, int i) {
	printf("Registering username and password...\n");
    FILE *db = fopen(DB_FILE, "ab");
    char stringPassword[16];
    //sprintf(stringPassword, "%lu", password);
    
    fprintf(db, "%s,%s,\n", username, password);    

    fclose(db);
    
    char response[1024];
    
    memcpy(response + 0, &(uint16_t)fd, 1);
    
    response[1] = 'r';
    
    memcpy(response + 2, &(uint16_t)Client[i].seq_number, 1);
    response[3] = '1';
    
    response[4] = '\0';
    
    write(fd, response, 1024);

    return 0;
}


int communicate(int socketfd) {
    
    // read data from socketfd

    // choose what to do with it

    return 0;
} 
*//*
void *process_received_messages(void *ClientDetail)
{
	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;

	while(1)
	{
		printf("Processing...\n");
		char data[1024];
		int n;
		read(clientSocket,data,1024);
		 // store the data sent from the client

		for(int i = 0; i < 50; i++)
			printf("%c\n", data[i]);

		uint16_t src = data[0];
		//memcpy(&src, data + 0, 1);

		printf("Value stored at position 0: %c\n", data[0]);
		printf("Source: %u\n", src);
		
		char c = data[1];

		char username[5];

		//uint16_t len = data[3];
		//memcpy(username, data + 4, len);
		//printf("User: %s\n", username);
	
		//printf("\n");
		//printf("%u\n", src);

		printf("\n");

		uint16_t len = data[3];
		printf("Length: %u\n", len);
		memcpy(username, data + 4, len);
		printf("User: %s\n", username);

		//int registered = 0;
		
		//while(registered == 0)
		//{
			//printf("Entering the authentication loop\n");

		printf("%c\n", data[1]);
			
		if(data[1] == 'c')
		{
			uint16_t username_len = data[3];

			char *username = malloc(sizeof(char) * (username_len + 1));
			strncpy(username, data + 5, username_len);

			uint16_t password_len = data[4];

			char *password = malloc(sizeof(char) * (password_len + 1));
			strncpy(password, data + 5 + username_len, password_len);
			
			char* response;
	
			printf("Before executing the function\n");
			response = checkCredentials(username, password, fd, clientCount);
			printf("After executing the function\n");
			//response[0] = '3';
			//response[1] = 'u';
			//response[2] = '2';
			//response[3] = '0';
			//response[4] = '\0';
			printf("Response is: %s\n", response);
			write(fd, response, 1024);
		}
		if(data[1] == 'u')
		{
			uint16_t username_len = data[3];
			char *username = malloc(sizeof(char) * (username_len + 1));

			strncpy(username, data + 4, username_len);

			char* response;
			response = checkUsername(username, fd, clientCount);
			//char response[1024];
			//response[0] = '3';
			//response[1] = 'u';
			//response[2] = '2';
			//response[3] = '0';
			//response[4] = '\0';
			//printf("Response is: %s\n", response);
			write(fd, response, 1024);
		}

		if(data[1] == 'r')
		{
				uint16_t username_len = data[3];

				char *username = malloc(sizeof(char) * (username_len + 1));
				strncpy(username, data + 5, username_len);

				uint16_t password_len = data[4];

				char *password = malloc(sizeof(char) * (password_len + 1));
				strncpy(password, data + 5 + username_len, password_len);

				registerCredentials(username, password);
		}

		//}

		if(data[1] == 'm')
		{
			for(int i = 0; i <= clientCount; i++) // send the message to all the other clients
			{
				if(Client[i].index != index)
				{
					//printf("Sending %s to client %i\n", data, index);
					write(Client[i].sockID, data, strlen(data));

				}
			}
		}
	}
	printf("Stopped processing...\n");
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

		for(int i = 0 ; i < clientCount ; i ++)
			pthread_join(thread[i],NULL);
   
	}

	//for(int i = 0 ; i < clientCount ; i ++)
		//pthread_join(thread[i],NULL);
	printf("Why are you like this?\n");

}*/
