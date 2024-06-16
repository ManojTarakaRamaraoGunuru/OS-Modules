#include "alloc.h"
#define BLOCKS PAGESIZE/MINALLOC

int is_free[BLOCKS];

void *base_ptr;

int init_alloc(){
    base_ptr = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

    if(base_ptr == MAP_FAILED){
        return 1;
    }
    for(int i = 0; i<BLOCKS; i++){
        is_free[i] = 1;
    }

    return 0;
}

void printer(){
    char* t = (char*)base_ptr;
    for(int i = 0; i<PAGESIZE; i++){
        printf("%c", t[i]);
    }
}

int cleanup(){
    
    int fd = munmap(base_ptr, PAGESIZE);
    if( fd == -1){
        return 1;
    }
    return 0;
}


char *alloc(int req_len){
    if(req_len % MINALLOC != 0){
        printf("MALLOC FAILED");
        return NULL;
    }

    int req_blocks = req_len/MINALLOC;

    int min_free_slot = PAGESIZE, free_start_idx = -1;

    for(int i = 0; i<BLOCKS; i++){
        int len = 0;
        if(is_free[i] == 1){
            for(int j = i; j<BLOCKS && is_free[j]==1; j++){
                len++;
            }
            
            if(req_blocks <= len && len <= min_free_slot){
                min_free_slot = len;
                free_start_idx = i;
            }
            i += (len-1);
        }
    }
    if(free_start_idx == -1)return NULL;
    for(int i = free_start_idx; i<free_start_idx + req_blocks; i++){
        is_free[i] = 0;
    }
    return (char*)(base_ptr + (free_start_idx*MINALLOC));
}

void dealloc(char *ptr){
    int idx = (int)(ptr - (char*)base_ptr);

    int len = sizeof(ptr);

}

int main(){
    init_alloc();
    char*arr = alloc(1024);
    char*arr1 = alloc(1024);
    int cnt = 0;
    dealloc(arr);
    dealloc(arr1);
    cleanup();
    return 0;
}