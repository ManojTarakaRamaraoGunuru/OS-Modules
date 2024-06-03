#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

// 

// Goal: Send an array from parent process to child process
// child process computes the product of numbers and returns to the parent
// parent process prints the product

int main(){
    int pipe1[2];
    if(pipe(pipe1) == -1){
        perror("pipe creation failed");
        return 1;
    }
    int n; scanf("%d\n", &n);
    
    int pid = fork();
    if(pid == 0){
        // child process
        close(pipe1[1]);
        printf("ok2 %d\n", n);
        int*arr = (int *)malloc(n*sizeof(int));

        if(read(pipe1[0],arr,sizeof(arr))){
            printf("Unable to write");
            return 1;
        }

        long long int prod = 1;
        for(int i = 0;i<n;i++){
            prod *= arr[i];
        }
        printf("%d", prod);
        close(pipe1[0]);

    }else{
        // parent process
        int* arr = (int*)malloc(n*sizeof(int));
        // int arr[100];
        for(int i = 0; i<n; i++){
            scanf("%d ", &arr[i]);
        }
        for(int i = 0; i<n+1; i++){
            printf("%d ", arr[i]);
        }
        close(pipe1[0]);
        printf("ok\n");
        if(write(pipe1[1],arr,n*sizeof(int))==-1){
            printf("Unable to write");
            return 1;
        }
        printf("ok1\n");
        close(pipe1[1]);
    }



    return 0;
}