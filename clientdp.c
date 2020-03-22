#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SIZE 256

struct sockaddr_in serv; //This is our main socket variable.
int fd; //This is the socket file descriptor that will be used to identify the socket
int conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[1024] = ""; //This array will store the messages that are sent by the server
int seq_number = 1;

/*void *receiving(void *sockID)
{
	int socket = *((int *) sockID);
	printf("Processing messages...\n");

	while(1)
	{
		char data[1024];
		int read = recv(socket, data, 1024, 0);
		data[read] = '\0';
		char source[12];
	
		strncpy(source, data, 12);
		int src = atoi(source);
		
		if(strlen(data + 25) != 0)
			printf("%i: %s\n", src, (data + 25));
	}
}*/

int openInterface() {
    // return some error code != 0 if encountered fatal problem
    return 0;
}

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; 

    return hash;
}

int checkCredentials(char* username, char* password, int clientSocket) {
    	unsigned long passwordHash = hash(password);

    	// check if username and passwordHash match on server
    	// if yes, return 0

	char response[1024];
	printf("Checking credentials...\n");
	// Aici construiesc pachetul pe care il trimit la server
	// Pachetul are urmatoarea structura: 2 bytes pentru sursa (care e file descriptorul), 1 byte pentru mode (ce tip de mesaj trimit pentru a sti serverul cum il proceseaza), 2 bytes pentru sequence number care numara mesajele transmise intre un client si server si restul pachetului e reprezentat de mesajul propriu-zis

	char data[1024];

	uint16_t socket = (uint16_t)clientSocket;
	memcpy(data + 0, &socket, 1);

	data[1] = 'c';

	uint16_t seq_num = (uint16_t)seq_number;
	memcpy(data + 2, &seq_num, 1);

	int username_len = strlen(username);
	int password_len = strlen(password);

	uint16_t usrn_len = (uint16_t) username_len;
	uint16_t pass_len = (uint16_t) password_len;

	memcpy(data + 3, &usrn_len, 1);
	memcpy(data + 4, &pass_len, 1);
	memcpy(data + 5, username, usrn_len);
	memcpy(data + 5 + usrn_len, password, pass_len);
	data[5 + usrn_len + pass_len] = '\0';


	// Trimit mesajul construit la server
	send(clientSocket, data, strlen(data), 0);
	seq_number++;
	
	// Astept raspuns de la server
	recv(clientSocket, response, 1024, 0);
	char recv_seq_num[12];
	
	strncpy(recv_seq_num, response + 13, 12);
	seq_number = atoi(recv_seq_num);
	
	char res = response[25];
	
	if(res == '0')
		return 0;
	else
		return -1;
}

int registerCredentials(char* username, char* password, int clientSocket) {
    unsigned long passwordHash = hash(password);

    // send new credentials to server to register (username and passwordHash)

	printf("Sending credentials to server...\n");
	char data[1024];
	
	uint16_t socket = (uint16_t)clientSocket;
	memcpy(data + 0, &socket, 1);

	data[1] = 'r';

	uint16_t seq_num = (uint16_t)seq_number;
	memcpy(data + 2, &seq_num, 1);

	int username_len = strlen(username);
	int password_len = strlen(password);

	uint16_t usrn_len = (uint16_t) username_len;
	uint16_t pass_len = (uint16_t) password_len;

	memcpy(data + 3, &usrn_len, 1);
	memcpy(data + 4, &pass_len, 1);
	memcpy(data + 5, username, usrn_len);
	memcpy(data + 5 + usrn_len, password, pass_len);
	data[5 + usrn_len + pass_len] = '\0';

	send(clientSocket, data, strlen(data), 0);
	seq_number++;


    return 0;
}

int checkUsername(char* username, int clientSocket) {                 
    // check with server if username is not already taken
    // if username is free, return 0
	printf("Checking username...\n");
	char response[1024];
	char data[1024];

	//char socket[12];
	//sprintf(socket, "%d", clientSocket);
	uint16_t socket = (uint16_t)clientSocket;
	memcpy(data + 0, &socket, 1);

	data[1] = 'u';

	//char seq_num[12];
	//sprintf(seq_num, "%d", seq_number);
	uint16_t seq_num = (uint16_t)seq_number;
	memcpy(data + 2, &seq_num, 1);

	int username_len = strlen(username);
	//char usrnm_len[12];

	uint16_t usrnm_len = (uint16_t)username_len;
	
	//sprintf(usrnm_len, "%d", username_len);
	memcpy(data + 3, &usrnm_len, 1);

	strcpy(data + 4, username);

	data[7 + username_len] = '\0';
	
	for(int i = 0; i < 50; i++)
		printf("%c\n", data[i]);

	printf("The packet for username checking sent to server is: %s\n", data);
	
	printf("Sending to server...\n");
	write(clientSocket, data, strlen(data));
	seq_number++;

	recv(clientSocket, response, 1024, 0);
	char recv_seq_num[12];
	
	strncpy(recv_seq_num, response + 13, 12);
	seq_number = atoi(recv_seq_num);
	
	char res = response[25];
	
	if(res == '0')
		return 0;
	else
		return -1;
	 
}

int signup(int clientSocket) {
    char* username = malloc(sizeof (char) * MAX_SIZE);
    char* password = malloc(sizeof (char) * MAX_SIZE);
    char* password2 = malloc(sizeof (char) * MAX_SIZE);
    
    printf("Enter username:\n");
    fgets(username, MAX_SIZE, stdin);
	printf("Username is: %s\n", username);
	checkUsername(username, clientSocket);
    if (checkUsername(username, clientSocket) != 0) {
        printf("Username not available!\n");
        return signup(clientSocket);
    }

    printf("Enter password:\n");
    fgets(password, MAX_SIZE, stdin);
    printf("Enter password again:\n");
    fgets(password2, MAX_SIZE, stdin);
    if (strcmp(password, password2) != 0) {
        printf("Passwords don't match!\n");
        return signup(clientSocket);
    }
    
    int success = registerCredentials(username, password, clientSocket);
    free(username); free(password); free(password2);
    if (success == 0)
        printf("User registered successfully. Login.\n");
    else
        printf("User couldn't be registered. Try again.\n");
    return success;
}

char* login(int clientSocket) {
    char* username = malloc(sizeof (char) * MAX_SIZE);
    char* password = malloc(sizeof (char) * MAX_SIZE);
    
    printf("Enter username:\n");
    fgets(username, MAX_SIZE, stdin);
    printf("Enter password:\n");
    fgets(password, MAX_SIZE, stdin);
    
    int check = checkCredentials(username, password, clientSocket);

    free(password);

    if (check == 0)
        return username;

    free(username);
    return (void *) NULL;
}

char* authenticate(int clientSocket) {
    printf("Login or Signup? (l/s)\n");
    int c = getchar(); getchar(); // consume newline
    
    if (c != 108 && c != 115)
        return authenticate(clientSocket);

    if (c == 115) {
        int success;
        while ((success = signup(clientSocket)) != 0);
    }
    return login(clientSocket);
}

void *receiving(void *sockID)
{
	int socket = *((int *) sockID);
	printf("Processing messages...\n");

	while(1)
	{
		char data[1024];
		int read = recv(socket, data, 1024, 0);
		data[read] = '\0';
		char source[12];
	
		strncpy(source, data, 12);
		int src = atoi(source);
		
		if(strlen(data + 25) != 0)
			printf("%i: %s\n", src, (data + 25));
	}
}

int main()
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8096);
	inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr); //This binds the client to localhost
	connect(fd, (struct sockaddr *)&serv, sizeof(serv)); //This connects the client to the server.

	char* auth = authenticate(fd);
    	if (auth == NULL) {
        	printf("Authentication failed!\n");
        	return -1;
    	}
    	else
        	printf("Authentication successful!\n");

    	printf("Starting chat...\n");

	pthread_t thread;
	pthread_create(&thread, NULL, receiving, (void *) &fd);
	
	while(1) 
	{
    		printf("Enter a message: ");
    		fgets((message + 25), 999, stdin);
		
		char socket[12];

		sprintf(socket, "%d", fd);

		strcpy(message, socket);

		message[12] = 'm';
		
		char seq_num[12];

		sprintf(seq_num, "%d", seq_number);
		strcpy(message + 13, seq_num);

		if(strlen(message + 25) != 0)
			printf("The message is: %s\n", (message + 7));
    		send(fd, message, strlen(message), 0);
		seq_number++;
		//strcpy(message, "");
		//An extra breaking condition can be added here (to terminate the while loop)
	}
}
