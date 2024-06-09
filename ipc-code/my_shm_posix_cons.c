// You can find the shared memory file at /dev/shm/MY_SHARED_MEMORY
// shm_unlink will delete that file, for validating what producer write before consumer 
// consumes, you can use this path

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#define SHM_SIZE 4096
#define SHM_NAME "MY_SHARED_MEMORY"

const char* free_str = "Freeeee\0";
const int offset = sizeof(free_str);

int main(){

	int shm_fd = shm_open(SHM_NAME, O_CREAT| O_RDWR, 0666);

	void*ptr = mmap(NULL,SHM_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

	if(ptr == (void*)-1){
		perror("mmap creation failed");
		return 1;
	}
	if(mkfifo("my_fifo",0666) == -1){
        if(errno != EEXIST){
            perror("unable to create fifo");
            return 1;
        }
    }
	
	int fifo_fd = open("my_fifo", O_RDONLY);

	int itr;
	for(int cnt = 0; cnt<1000; cnt++){
		if(read(fifo_fd, &itr, sizeof(itr)) == -1){
			perror("unable to read");
			return 1;
		}
		printf("%s - %d\n",ptr+(itr*offset), itr);
		sprintf(ptr+(itr * offset), "%s", free_str);
		usleep(100000);
	}
	close(fifo_fd);

	if(shm_unlink(SHM_NAME) == -1){
		perror("unable to delete shared memory");
		return 1;
	}
	return 0;
}
