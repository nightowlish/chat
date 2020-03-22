#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define LINE_SIZE 1024
#define DB_FILE "db.csv"

struct sockaddr_in serv; //This is our main socket variable.
int fd; //This is the socket file descriptor that will be used to identify the socket
int conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[1024] = ""; //This array will store the messages that are sent by the server
int clientCount = 0;

struct client{

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

int checkUsername(char* username, int socket, int i) {
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
	char socket_num[12];
	char seq_num[12];
	
	sprintf(socket_num, "%d", socket);
	strcpy(response, socket_num);
	response[12] = 'u';

	sprintf(seq_num, "%d", Client[i].seq_number);
	strcpy(response + 13, seq_num);	
	
    	if(found)
	{
		response[25] = '1';
		printf("Response for username check: %s\n", response);
		send(socket, response, 1024, 0);
        	return -1;
	}
    	else
	{
		response[25] = '0';
		send(socket, response, 1024, 0);
        	return 0;
	}
}

int checkCredentials(char* username, unsigned long password, int socket, int i) {
		printf("Credential checking done on the server side...\n");
    	int found = 0;
    	char line[LINE_SIZE];
    	FILE *db = fopen(DB_FILE, "r");

    	char stringPassword[16];
    	sprintf(stringPassword, "%lu", password);
    
    	while (fgets(line, LINE_SIZE, db)) {
        	char* tmp = strdup(line); 
        	char* tmp2 = strdup(line);
        
        	char* usernameDB = getFieldFromLine(tmp, 1);
        	char* passwordDB = getFieldFromLine(tmp2, 2);

        	if (strcmp(usernameDB, username) == 0 && strcmp(passwordDB, stringPassword) == 0) {
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
	char socket_num[12];
	char seq_num[12];
	
	sprintf(socket_num, "%d", socket);
	strcpy(response, socket_num);

	response[12] = 'c';

	sprintf(seq_num, "%d", Client[i].seq_number);
	strcpy(response + 13, seq_num);
	

  	if(found)
	{
		response[25] = '0';
		send(socket, response, 1024, 0);
        	return 0;
	}
    	else
	{
		response[25] = '1';
		send(socket, response, 1024, 0);
        	return -1;
	}
}

int registerCredentials(char* username, unsigned long password) {
	printf("Registering username and password...\n");
    FILE *db = fopen(DB_FILE, "ab");
    char stringPassword[16];
    sprintf(stringPassword, "%lu", password);
    
    fprintf(db, "%s,%s,\n", username, stringPassword);    

    fclose(db);
    return 0;
}


int communicate(int socketfd) {
    
    // read data from socketfd

    // choose what to do with it

    return 0;
} 

void *process_received_messages(void *ClientDetail)
{
	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;

	while(1)
	{
		char data[1024];
		int n;
		read(clientSocket,data,1024); // store the data sent from the client
		uint16_t src;
		memcpy(&src, data + 0, 1);
		
		char c = data[1];

		char username[5];

		/*uint16_t len = data[3];
		memcpy(username, data + 4, len);
		printf("User: %s\n", username);*/
	
		printf("\n");
		printf("%u\n", src);

		printf("\n");

		uint16_t len = data[3];
		printf("Length: %u\n", len);
		memcpy(username, data + 4, len);
		//printf("User: %s\n", username);

		int registered = 0;
		
		while(registered == 0)
		{
			//printf("Entering the authentication loop\n");
			
			if(data[12] == 'c')
			{
				char username_len[12];
			
				strncpy(username_len, data + 25, 12);
				int usrnm_len = atoi(username_len);

				char *username = malloc(sizeof(char) * (usrnm_len + 1));
				strncpy(username, data + 49, usrnm_len);

				char password_len_string[12];
			
				strncpy(password_len_string, data + 37, 12);
				int password_len = atoi(password_len_string);

				char *password_string = malloc(sizeof(char) * (password_len + 1));
				strncpy(password_string, data + 49, password_len);
				int password = atoi(password_string);
				
				checkCredentials(username, (unsigned long) password, fd, clientCount);
				break;

			}
			if(data[12] == 'u')
			{
				char username_len[12];
				strncpy(username_len, data + 25, 12);
				int usrnm_len = atoi(username_len);
				char *username = malloc(sizeof(char) * (usrnm_len + 1));

				strcpy(username, data + 25);
				checkUsername(username, fd, clientCount);
			}

			if(data[12] == 'r')
			{
				char username_len[12];
			
				strncpy(username_len, data + 25, 12);
				int usrnm_len = atoi(username_len);

				char *username = malloc(sizeof(char) * (usrnm_len + 1));
				strncpy(username, data + 49, usrnm_len);

				char password_len_string[12];
			
				strncpy(password_len_string, data + 37, 12);
				int password_len = atoi(password_len_string);

				char *password_string = malloc(sizeof(char) * (password_len + 1));
				strncpy(password_string, data + 49, password_len);
				int password = atoi(password_string);

				registerCredentials(username, (unsigned long) password);
				break;
			}
		}


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

		/*int registered = 0;
		
		while(registered == 0)
		{
			//printf("Entering the authentication loop\n");
			char data[1024];

			recv(Client[clientCount].sockID, data, 1024, 0);
			
			if(data[12] == 'c')
			{
				char username_len[12];
			
				strncpy(username_len, data + 25, 12);
				int usrnm_len = atoi(username_len);

				char *username = malloc(sizeof(char) * (usrnm_len + 1));
				strncpy(username, data + 49, usrnm_len);

				char password_len_string[12];
			
				strncpy(password_len_string, data + 37, 12);
				int password_len = atoi(password_len_string);

				char *password_string = malloc(sizeof(char) * (password_len + 1));
				strncpy(password_string, data + 49, password_len);
				int password = atoi(password_string);
				
				checkCredentials(username, (unsigned long) password, fd, clientCount);
				break;

			}
			if(data[12] == 'u')
			{
				char username_len[12];
				strncpy(username_len, data + 25, 12);
				int usrnm_len = atoi(username_len);
				char *username = malloc(sizeof(char) * (usrnm_len + 1));

				strcpy(username, data + 25);
				checkUsername(username, fd, clientCount);
			}

			if(data[12] == 'r')
			{
				char username_len[12];
			
				strncpy(username_len, data + 25, 12);
				int usrnm_len = atoi(username_len);

				char *username = malloc(sizeof(char) * (usrnm_len + 1));
				strncpy(username, data + 49, usrnm_len);

				char password_len_string[12];
			
				strncpy(password_len_string, data + 37, 12);
				int password_len = atoi(password_len_string);

				char *password_string = malloc(sizeof(char) * (password_len + 1));
				strncpy(password_string, data + 49, password_len);
				int password = atoi(password_string);

				registerCredentials(username, (unsigned long) password);
				break;
			}
		}*/

		pthread_create(&thread[clientCount], NULL, process_received_messages, (void *) &Client[clientCount]);
		
		clientCount++;
   
	}

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);

}
