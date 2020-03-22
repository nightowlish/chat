#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <errno.h>
#include <stdbool.h>

#define LINE_SIZE 1024
#define DB_FILE "db.csv"

/*
#include <netdb.h>
#include <netinet/in.h>

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int n, pid;
   
   sockfd = socket(AF_INET, SOCK_STREAM, 0); 
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr)); //seteaza toti bitii 0 ai structurii serv_addr
   portno = 5001;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   //* Now bind the host address using bind() call.*
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
   
   // Now start listening for the clients, here process will go in sleep mode and will wait for the incoming connection
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while (1) {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        
      if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
      }
      
      //* Create child process *
      pid = fork();
        
      if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
      }
      
      if (pid == 0) {
         //* This is the client process *
         close(sockfd);
         doprocessing(newsockfd);
         exit(0);
      }
      else {
         close(newsockfd);
      }
        
   } 
}*/

char* getFieldFromLine(char* line, int fieldIndex) {
    char* tokenPtr;
    for (tokenPtr = strtok(line, ","); tokenPtr && *tokenPtr; tokenPtr = strtok(NULL, ",\n")) {
        if (!--fieldIndex)
            return tokenPtr;
    }
    return NULL;
}

int checkUsername(char* username) { 
    bool found = false;
    char line[LINE_SIZE];
    FILE *db = fopen(DB_FILE, "r");
    
    while (fgets(line, LINE_SIZE, db)) {
        char* tmp = strdup(line); 

        char* usernameDB = getFieldFromLine(tmp, 1);

        if (strcmp(usernameDB, username) == 0) {
            found = true;
            free(tmp);
            break;
        }

        free(tmp); // strtok clobbers temp
    }

    fclose(db);
    if(found)
        return -1;
    else
        return 0;
}

int checkCredentials(char* username, unsigned long password) {
    bool found = false;
    char line[LINE_SIZE];
    FILE *db = fopen(DB_FILE, "r");

    char stringPassword[16];
    itoa(password, stringPassword, 10);
    
    while (fgets(line, LINE_SIZE, db)) {
        char* tmp = strdup(line); 
        char* tmp2 = strdup(line);
        
        char* usernameDB = getFieldFromLine(tmp, 1);
        char* passwordDB = getFieldFromLine(tmp2, 2);

        if (strcmp(usernameDB, username) == 0 && strcmp(passwordDB, stringPassword) == 0) {
            found = true;
            free(tmp);
            free(tmp2);
            break;
        }

        // strtok clobbers temp
        free(tmp); 
        free(tmp2);
    }

    fclose(db);
    if(found)
        return 0;
    else
        return -1;
}

int registerCredentials(char* username, unsigned long password) {
    FILE *db = fopen(DB_FILE, "ab");
    char stringPassword[16];
    itoa(password, stringPassword, 10);
    
    fprintf(db, "%s,%s,\n", username, stringPassword);    

    fclose(db);
    return 0;
}


int communicate(int socketfd) {
    
    // read data from socketfd

    // choose what to do with it

    return 0;
} 


int main() {
    int socketfd, newsocketfd = 0;

    // create socketfd

    // bind socketfd

    // listen(sockfd)

    while(true) {

        // newsocketfd = accept() 

        // fork

        // communicate(newsocketfd);

        // exit()   
        break;         
    }            
    
    // =============== DEBUG ==================
    // printf("%d\n", checkUsername("test\0"));
    // printf("%d\n", checkCredentials("test\0", 1234567890));
    // printf("%d\n", registerCredentials("test\0", 1234));
    
}


