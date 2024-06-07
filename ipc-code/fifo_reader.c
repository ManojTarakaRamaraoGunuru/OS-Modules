#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

// Unlike pipes that can estabilish communication between processes of same hierarchy, named pipes(fifo)
// can establish communication between processes of different hierarchies

int main(){
    if(mkfifo("my_fifo", 0777) == -1){
        // printf("%s", strerror(errno));
        if(errno != EEXIST){
            printf("Unable to create fifo");
            return 1;
        }
    }
    
    int fd = open("my_fifo",O_RDONLY);

    int x;
    
    if(read(fd, &x, sizeof(int))==-1){
        printf("Issue at reading");
        return 1;
    }

    printf("%d", x);

    close(fd);


    return 0;
}