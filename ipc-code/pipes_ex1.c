#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

// pipes are one way communication channels, if you don't close one end (read or write) before you do another end operation,
// same process can do both ends (read and write) togethe makes other process wait in a blocking call

// Goal: Send an array from parent process to child process
// child process computes the product of numbers and returns to the parent
// parent process prints the product

int main(){
    int fd1[2]; // parent process writes and child process reads
    int fd2[2]; // child process writes and parent process reads

    if(pipe(fd1) == -1){
        perror("pipe creation failed");
        return 1;
    }
    if(pipe(fd2) == -1){
        perror("pipe creation failed");
        return 1;
    }

    int n; scanf("%d", &n);

    int pid = fork();
    if(pid == 0){
        // child process
        close(fd1[1]);
        close(fd2[0]);

        int*arr = (int *)malloc(n*sizeof(int));
        if(arr == NULL){
            perror("malloc failed");
            return 1;
        }

        if(read(fd1[0],arr,n*sizeof(int)) == -1){
            printf("Unable to read");
            return 1;
        }

        long long int prod = 1;
        for(int i = 0;i<n;i++){
            prod *= arr[i];
        }
        free(arr);

        if(write(fd2[1],&prod,sizeof(long long int)) == -1){
            perror("Unable to write");
            return 1;
        }

        close(fd2[1]);
        close(fd1[0]);

    }else{
        // parent process
        int* arr = (int*)malloc(n*sizeof(int)); 
        if(arr == NULL){
            perror("malloc failed");
            return 1;
        }

        for(int i = 0; i<n; i++){
            scanf("%d", &arr[i]);
        }
        
        close(fd1[0]);
        close(fd2[1]);

        if(write(fd1[1],arr,n*sizeof(int)) == -1){
            perror("Unable to write");
            return 1;
        }
        free(arr);

        long long prod;
        if(read(fd2[0],&prod,sizeof(long long int)) == -1){
            printf("Unable to read");
            return 1;
        }

        printf("Product is : %lld\n", prod);

        close(fd2[0]);
        close(fd1[1]);
        waitpid(pid, NULL, 0);     // to reap the child process
    }



    return 0;
}