#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SHM_SIZE 4096
#define SHM_NAME "MY_SHARED_MEMORY"

// Goal: write "OSisFun\n" 8 bytes string 1000 times so that consumer can consume the string in 4KB shared memory
// producer can write 512 copies of the string in the shared memory at one go, it needs to reuse the memory to rewrite the remaining
// For that, there needs to be another way of communication of processes that is achieved through named pipes or fifo

int main(){
    const char* str = "OSisFun\n";

    // open shared memory with read and write mode with 666 permission
    int shm_fd = shm_open(SHM_NAME,O_CREAT | O_RDWR, 0666);
    // configure the size of the memory
    ftruncate(shm_fd, SHM_SIZE);

    void*ptr = mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd,0);

    if(ptr == (void *)-1){
        printf("Creation of memory mapping failed");
        return 1;
    }

    sprintf(ptr,"%s",str);
    ptr += sizeof(str);
    sprintf(ptr,"%s",str);
    ptr += sizeof(str);    

    return 0;
}
