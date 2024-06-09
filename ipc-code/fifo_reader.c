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
// program of writer or reader will not terminate unless the other also consumes

int main(){
    if(mkfifo("my_fifo", 0777) == -1){
        // printf("%s", strerror(errno));
        if(errno != EEXIST){
            perror("Unable to create fifo");
            return 1;
        }
    }
    
    int fd = open("my_fifo",O_RDONLY);

    int n;
    
    if(read(fd, &n, sizeof(int))==-1){
        printf("Issue at reading");
        return 1;
    }
    printf("%d\n", n);

    // reading numbers from the array that were sent by producer
    int x;
    for(int i = 0; i<n; i++){
        if(read(fd, &x, sizeof(int))==-1){
            perror("Issue at reading");
            return 1;
        }
        printf("%d ",x);
    }

    close(fd);


    return 0;
}