#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

#define MAX_SIZE 256

/*
#include <netdb.h> 
#include <sys/socket.h> 
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 


void func(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    for (;;) { 
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
        write(sockfd, buff, sizeof(buff)); 
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        printf("From Server : %s", buff); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            break; 
        } 
    } 
} 

int main() 
{ 
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 

    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 

    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 

    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 

    // function for chat 
    func(sockfd); 

    // close the socket 
    close(sockfd); 
} 
*/

int openInterface() {
    // return some error code != 0 if encountered fatal problem
    return 0;
}

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; 

    return hash;
}

int checkCredentials(char* username, char* password) {
    unsigned long passwordHash = hash(password);

    // check if username and passwordHash match on server
    // if yes, return 0

    return 0;
}

int registerCredentials(char* username, char* password) {
    unsigned long passwordHash = hash(password);

    // send new credentials to server to register (username and passwordHash)

    return checkCredentials(username, password);
}

int checkUsername(char* username) {                 
    // check with server if username is not already taken
    // if username is free, return 0 
    return 0;
}

int signup() {
    char* username = malloc(sizeof (char) * MAX_SIZE);
    char* password = malloc(sizeof (char) * MAX_SIZE);
    char* password2 = malloc(sizeof (char) * MAX_SIZE);
    
    printf("Enter username:\n");
    gets(username);
    if (checkUsername(username) != 0) {
        printf("Username not available!\n");
        return signup();
    }

    printf("Enter password:\n");
    gets(password);
    printf("Enter password again:\n");
    gets(password2);
    if (strcmp(password, password2) != 0) {
        printf("Passwords don't match!\n");
        return signup();
    }
    
    int success = registerCredentials(username, password);
    free(username); free(password); free(password2);
    if (success == 0)
        printf("User registered successfully. Login.\n");
    else
        printf("User couldn't be registered. Try again.\n");
    return success;
}

char* login() {
    char* username = malloc(sizeof (char) * MAX_SIZE);
    char* password = malloc(sizeof (char) * MAX_SIZE);
    
    printf("Enter username:\n");
    gets(username);
    printf("Enter password:\n");
    gets(password);
    
    int check = checkCredentials(username, password);

    free(password);

    if (check == 0)
        return username;

    free(username);
    return (void *) NULL;
}

char* authenticate() {
    printf("Login or Signup? (l/s)\n");
    int c = getchar(); getchar(); // consume newline
    
    if (c != 108 && c != 115)
        return authenticate();

    if (c == 115) {
        int success;
        while ((success = signup()) != 0);
    }
    return login();
}

int main() {

    char* auth = authenticate();
    if (auth == NULL) {
        printf("Authentication failed!\n");
        return -1;
    }
    else
        printf("Authentication successful!\n");

    printf("Starting chat...\n");
    return openInterface();
}
    