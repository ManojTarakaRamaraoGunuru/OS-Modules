#include "ealloc.h"
#define BLOCKS PAGESIZE/MINALLOC
#define PAGES 4

int is_free[PAGES][BLOCKS];
int alloc_blocks_len[PAGES][BLOCKS];
int is_page_initialized[PAGES];
void *base_ptr[PAGES];
int page_number;


void init_alloc(){
    
    for(int i = 0; i<PAGES; i++){
        is_page_initialized[i] = 0;
    }
    for(int j=0; j<PAGES; j++){
        for(int i = 0; i<BLOCKS; i++){
            is_free[j][i] = 1;
        }
    }
    for(int j=0; j<PAGES; j++){
        for(int i = 0; i<BLOCKS; i++){
            alloc_blocks_len[j][i] = 0;
        }
    }
    for(int i = 0; i<PAGES; i++)base_ptr[i] = NULL;
    page_number = -1;
}



void cleanup(){
    return;
}

int get_free_start_idx(int page_number, int req_len, int req_blocks){
    

    int min_free_slot = PAGESIZE, free_start_idx = -1;
    for(int i = 0; i<BLOCKS; i++){
        int len = 0;
        if(is_free[page_number][i] == 1){
            for(int j = i; j<BLOCKS && is_free[page_number][j]==1; j++){
                len++;
            }
            
            if(req_blocks <= len && len <= min_free_slot){
                min_free_slot = len;
                free_start_idx = i;
            }
            i += (len-1);
        }
    }
    return free_start_idx;
}

// Alloc function gives the smallest best fit block (>= required length)

char *alloc(int req_len){
    // fflush(stdout);
    if(req_len % MINALLOC != 0){
        printf("MALLOC FAILED");
        return NULL;
    }
    
    if(page_number == -1){
        page_number++;
        base_ptr[page_number] = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    }

    int req_blocks = req_len/MINALLOC;
    int free_start_idx = get_free_start_idx(page_number, req_len, req_blocks);
    if(free_start_idx == -1){
        page_number++;
        if(page_number == PAGES){
            return NULL;
        }
        free_start_idx = get_free_start_idx(page_number, req_len, req_blocks);
        base_ptr[page_number] = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    }

    if(free_start_idx == -1)return NULL;
    for(int i = free_start_idx; i<(free_start_idx + req_blocks); i++){
        is_free[page_number][i] = 0;
    }
    

    alloc_blocks_len[page_number][free_start_idx] = req_blocks;
    return (char*)(base_ptr[page_number] + (free_start_idx*MINALLOC));
    
}

// Merging adjacent free blocks is possible with is_free arr
void dealloc(char *ptr){
    printf("%d\n", ptr);
    // int idx = (int)(ptr - (char*)base_ptr)/MINALLOC;

    // int len = alloc_blocks_len[page_number][idx];
    // alloc_blocks_len[page_number][idx] = 0;

    // for(int i = idx; i<(idx + len); i++){
    //     is_free[page_number][i] = 1;
    // }

}