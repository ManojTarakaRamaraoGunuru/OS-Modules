#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

// Goal : Colect an array from user input and send it to the other process using fifo

int main(){
    if(mkfifo("my_fifo", 0777) == -1){
        // printf("%s", strerror(errno));
        if(errno != EEXIST){
            printf("Unable to create fifo");
            return 1;
        }
    }
    
    int fd = open("my_fifo",O_WRONLY);
    if (fd == -1) {
        perror("Error opening FIFO");
        return 1;
    }

    int n; scanf("%d",&n);
    if(write(fd, &n, sizeof(int)) == -1){
        printf("Issue at writing");
        return 1;
    }

    int* arr = (int *)malloc(sizeof(int));
    for(int i = 0; i<n; i++){
        scanf("%d", &arr[i]);
        printf("%d ",arr[i]);
        if(write(fd, &arr[i], sizeof(int)) == -1){
            printf("Issue at writing");
            return 1;
        }
    }
    free(arr);

    close(fd);


    return 0;
}