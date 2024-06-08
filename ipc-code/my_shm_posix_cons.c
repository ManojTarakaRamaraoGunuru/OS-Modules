#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_SIZE 4096
#define SHM_NAME "MY_SHARED_MEMORY"

// You can find the shared memory file at /dev/shm/MY_SHARED_MEMORY
// shm_unlink will delete that file, for validating what producer write before consumer 
// consumes, you can use this path

int main(){

	int shm_fd = shm_open(SHM_NAME, O_CREAT| O_RDONLY, 0666);

	void*ptr = mmap(0,SHM_SIZE,PROT_READ, MAP_SHARED, shm_fd, 0);

	if(ptr == (void*)-1){
		printf("Error in reaching shared memory");
		return 1;
	}

	printf("%s", (char*)ptr);

	if(shm_unlink(SHM_NAME) == -1){
		printf("problem in closing shared memory");
		return 1;
	}
	return 0;
}
