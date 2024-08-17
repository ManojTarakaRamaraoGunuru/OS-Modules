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
#define MAX_KEYS 20

/*
    socket maintainence related information
*/

int sockfd, newsockfd, portno, clilen;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int n1, n2;
char** tokens;

/*
    Data storage variables information
*/
int key_arr[MAX_KEYS];
int value_len[MAX_KEYS];
char* value_arr[MAX_KEYS];
int idx = 0;

void error(char *msg){
    perror(msg);
    exit(0);
}

int make_server(char *argv[]){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* fill in port number to listen on. IP address can be anything (INADDR_ANY) */

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* bind socket to this port number on this machine */

    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0) 
            error("ERROR on binding");
    
    /* listen for incoming connection requests */

     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     /* accept a new request, create a newsockfd */

     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
        error("ERROR on accept");
    return 1;
}

int tokenize(){
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

int send_msg(char* msg){
    n2 = write(newsockfd,msg,100);
    if(n2<0)error("ERROR writing from socket");
    return n2;
}

char* create_key_value_pair(){
    /*
        create <key> <value-size> <value>    
    */
   int key = atoi(tokens[1]);
   if(idx == MAX_KEYS){
    return "storage full";
   }
   for(int i = 0; i<idx; i++){
    if(key_arr[i] == key){
        return "already key exists";
    }
   }
    key_arr[idx] = key;
    value_len[idx] = atoi(tokens[2]);
    value_arr[idx] = (char*)malloc(sizeof(char)*value_len[idx]);
    strcpy(value_arr[idx], tokens[3]);
    // printf("Added key:%d Value:%s Size:%d\n", key_arr[idx], value_arr[idx], value_len[idx]);
    idx++;
    return "Key Value pair Added";
}

char* Read(){
    // read <key>
    int key  = atoi(tokens[1]);
    for(int i = 0; i<idx; i++){
        if(key_arr[i] == key){
            return value_arr[i];
        }
    }
    return "No such key";
}

char* update(){
    // update <key> <value-size> <value>
    int key = atoi(tokens[1]);

    for(int i = 0; i<idx; i++){
        if(key_arr[i] == key){
            free(value_arr[i]);
            value_len[i] = atoi(tokens[2]);
            value_arr[i]= (char*)malloc(sizeof(value_len[idx]));
            strcpy(value_arr[i], tokens[3]);
            return "Updated";
        }
   }
   return "No such key";
}

char* delete(){
    //delete <key>
    int key = atoi(tokens[1]);

    for(int i = 0; i<idx; i++){
        if(key_arr[i] == key){
            free(value_arr[i]);
            key_arr[i] = -1;
            value_len[i] = -1;
            return "Deleted key";
        }
   }
   return "No such key";
}

void disconnect(char*argv[]){
    if (newsockfd >= 0) {
        close(newsockfd);
        newsockfd = -1; // Reset to an invalid value
    }

    // Close the main server socket if needed
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1; // Reset to an invalid value
    }
    make_server(argv);
}

int main(int argc, char *argv[]){
    fflush(stdout);
    make_server(argv);
    do{
        bzero(buffer,TOKEN_SIZE);
        tokens = (char**)malloc(sizeof(char*)*MAX_TOKENS);
        n1 = read(newsockfd,buffer,TOKEN_SIZE);
        
        if(n1<0)error("ERROR reading from socket");
        printf("Here is the message: %s ",buffer);

        int len = tokenize();

        if(strcmp(tokens[0], "create") == 0){
            char*msg = create_key_value_pair();
            n2 = send_msg(msg);
        }
        else if(strcmp(tokens[0], "read") == 0){
            char*msg = Read();
            n2 = send_msg(msg);
        }
        else if(strcmp(tokens[0], "update")== 0){
            char*msg = update();
            n2 = send_msg(msg);
        }
        else if(strcmp(tokens[0], "delete") == 0){
            char*msg = delete();
            n2 = send_msg(msg);
        }
        // else if(strcmp(tokens[0], "disconnect") == 0){
        //     n2 = send_msg("disconnecting");
        //     disconnect(argv);
        // }
        else{
            // n2 = write(newsockfd,"I got your message",18);
            // if (n2 < 0) error("ERROR writing to socket");
            n2 = send_msg("I got your message");
        }
        free(tokens);

    }while(n1>0 &&  n2>0);

    return 0;
}