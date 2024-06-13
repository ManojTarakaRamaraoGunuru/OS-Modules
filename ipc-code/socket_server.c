#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "unix_socket_example"
#define BUFFER_SIZE 256

void error(char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    fflush(stdout);  // Ensure the message is immediately printed

    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_un serv_addr, cli_addr;
    int n;

    /* Create socket */

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* Remove any existing socket file */
    unlink(SOCK_PATH);

    /* Fill in socket address */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCK_PATH);

    /* Bind socket to this address */
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    /* Read message from client */

    bzero(buffer, BUFFER_SIZE);
    int len = sizeof(cli_addr);

    printf("Server ready\n");

    while (1) {
        n = recvfrom(sockfd, buffer, BUFFER_SIZE-1, 0, (struct sockaddr *)&cli_addr, &len);
        printf("%d\n", n);
        if (n < 0) {
            error("ERROR reading from socket");
        }else if(n<BUFFER_SIZE){
            break;
        }
    }
    printf("%s", buffer);

    unlink(SOCK_PATH);
    return 0; 
}
