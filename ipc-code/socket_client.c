#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#define SOCK_PATH "unix_socket_example"
#define BUFFER_SIZE 256

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
    int sockfd, portno, n;

    /* create socket, get sockfd handle */
    struct sockaddr_un serv_addr;
    char buffer[BUFFER_SIZE];

    /* create socket, get sockfd handle */
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* fill in server address */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCK_PATH);

    /* ask user for input */
    printf("Please enter the path of the file: ");
    char*path = (char*)malloc(BUFFER_SIZE*sizeof(char)); scanf("%s", path);
    bzero(buffer,BUFFER_SIZE);

    int fd = open(path, O_RDONLY);

    if(fd == -1){
        perror("Issue in opening the file");
        return 1;
    }

    while(1){

        int _read_bytes = read(fd, buffer, sizeof(buffer));

        if(_read_bytes == -1){
            perror("Issue at reading");
            return 1;
        }
        else if(_read_bytes == 0){
            break;
        }

        printf("Sending data...\n");
        n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

        if (n < 0) 
            error("ERROR writing to socket");

    }

    
    

    /* send user message to server */
    
    free(path);
    close(fd);
    close(sockfd);
    return 0;

    return 0;
}