#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#define MAX_LEN 1000

// pipes are shared memory buffers used for communication between processes of the same hierarchy
// named pipes or fifos are used communication between processes of different hierarchy but concept of both remains the same
// when you create a pipe and fork, the same pipe will be inherited to all processes
// A pipe has 2 ends, read end and write end that are stored in fd[0], fd[1] respectively, where fds are file descriptors
// when you do read action at one end, make sure you close other end, vice versa otherwise other process will not move further


// Goal: write your name into the child process and read it from the main process
// After reading in the main process, send a confirmation signal to child process

int main(){
    int fd[2];
    if(pipe(fd) == -1){
        printf("Error in creation of pipe");
        return 1;
    }
    int len; 
    int pid = fork();
    if(pid == 0){
        scanf("%d\n", &len);
        char arr[MAX_LEN];
        for(int  i = 0; i<len; i++){
            scanf("%c", &arr[i]);
        }
        close(fd[0]); // close read descriptor in the child process
        if(write(fd[1], arr, len*sizeof(char)) == -1){
            printf("Issue in writing into the buffer\n");
            return 1;
        }
        close(fd[1]); // write finished, close write descriptor

    }else{
        char arr[MAX_LEN];
        close(fd[1]); // close write descriptor in the main process
        if(read(fd[0], arr, len*sizeof(char)) == -1){   // also acts as blocking system call until child process writes into buffer
            printf("Issue in Reading from the buffer\n");
            return 1;
        }
        printf("%s", arr);
        close(fd[0]); // read finished, close read descriptor

        wait(NULL); // reap the child process
    }
    return 0;
}