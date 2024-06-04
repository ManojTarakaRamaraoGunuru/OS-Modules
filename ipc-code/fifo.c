#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(){
    if(mkfifo("my_fifo", 0777) == -1){
        if(errno != EEXIST){
            printf("Unable to create fifo");
        }
        return 1;
    }
    
    int fd = open("my_fifo",O_WRONLY);

    int x = 97;
    
    if(write(fd, &x, sizeof(int)) == -1){
        printf("Issue at writing");
        return 1;
    }

    close(fd);


    return 0;
}