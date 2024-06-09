// Goal: write "OSisFun\n" 8 bytes string 1000 times so that consumer can consume the string in 4KB shared memory
// producer can write 512 copies of the string in the shared memory at one go, it needs to reuse the memory to rewrite the remaining
// For that, there needs to be another way of communication of processes that is achieved through named pipes or fifo

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

#define SHM_SIZE 4096
#define SHM_NAME "MY_SHARED_MEMORY"

const char* str = "OSisFun\0";
const char* free_str = "Freeeee\0";
const int offset = sizeof(free_str);


int main(){
    

    // open shared memory with read and write mode with 666 permission
    int shm_fd = shm_open(SHM_NAME,O_CREAT | O_RDWR, 0666);
    // configure the size of the memory
    ftruncate(shm_fd, SHM_SIZE);

    void*ptr = mmap(NULL, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd,0);
    if(ptr == (void *)-1){
        perror("mmap creation failed");
        return 1;
    }

    if(mkfifo("my_fifo",0666) == -1){
        if(errno != EEXIST){
            perror("unable to create fifo");
            return 1;
        }
    }
    for(int i = 0; i<512; i++){
        sprintf(ptr + (i*offset),"%s",free_str);
    }

    int fifo_fd = open("my_fifo",O_WRONLY); 

    int itr = 0;
    for(int cnt = 0; cnt<1000; cnt++){
        if(cnt == 512){
            itr = 0;
        }
        while(strcmp(ptr+(itr*offset),free_str) != 0){};

        if(strcmp(ptr+(itr*offset),free_str) == 0){
            sprintf(ptr + itr*offset, "%s", str);
            printf("%s - %d\n", ptr+(itr*offset), cnt+1);
            if(write(fifo_fd,&itr,sizeof(itr)) == -1){
                printf("write to fifo failed");
                return 1;
            }
        }
        itr++;
    }

    close(fifo_fd);

    

    return 0;
}
