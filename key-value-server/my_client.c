#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <sys/un.h>
#include <string.h>

#define MAX_TOKENS 20
#define TOKEN_SIZE 20

char**tokens;
struct sockaddr_in serv_addr;
struct hostent *server;
int sockfd, portno;

void error(char *msg){
    perror(msg);
    exit(0);
}


int tokenize(char buffer[], int len){

    tokens = (char**)malloc(MAX_TOKENS * sizeof(char*));
    if (!tokens) {
        error("Memory allocation failed");
    }

    int tokenNo = 0, tokenIndex = 0;
    char* token = strtok(buffer, " \t\n");
    
    while (token != NULL && tokenNo < MAX_TOKENS) {
        tokens[tokenNo] = (char*)malloc((strlen(token) + 1) * sizeof(char));
        if (!tokens[tokenNo]) {
            error("Memory allocation failed");
        }
        strcpy(tokens[tokenNo], token);
        tokenNo++;
        token = strtok(NULL, " \t\n");
    }
    
    return tokenNo;
}


int make_connetion(){
    /*connect <server-ip> <server-port>*/

    /* create socket, get sockfd handle */
    portno = atoi(tokens[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    /* fill in server address in sockaddr_in datastructure */
    server = gethostbyname(tokens[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    /* connect to server */

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    return 1;
}

void send_token_information(int len){
    for(int i = 0; i<len; i++){
        char buffer[256];
        bzero(buffer, 256);
        strcpy(buffer, tokens[i]);
        
    }
}


int main(){
    fflush(stdout);
    

    char buffer[256];
    while(1){
        /*Client cannot proceed frther until it make a successful connection with the server*/
        /* input format : connect <server> <port> */
        printf("Enter the message:");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        int len =  tokenize(buffer, 256);
        if(strcmp(tokens[0], "connect") == 0 && make_connetion() == 1){
            printf("Ok\n");
            break;
        }
        else printf("connection error");
    }

    while(1){
        printf("Enter the message:");
        char buffer[256];
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

        int n = write(sockfd, buffer, TOKEN_SIZE);
        int len =  tokenize(buffer, 256);
        if(n<0)error("write failure");

        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        printf("Received from server: %s\n", buffer);

        free(tokens);
    }
    return 0;
}