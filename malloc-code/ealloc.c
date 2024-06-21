#include "ealloc.h"
#define BLOCKS PAGESIZE/MINALLOC
#define PAGES 4

int is_free[PAGES][BLOCKS];
int alloc_blocks_len[PAGES][BLOCKS];
void *base_ptr[PAGES];


// initialising required data structures to manage demanded memory
// but demanding pages only based on requirement from alloc
void init_alloc(){
    
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
    if(req_len % MINALLOC != 0){
        printf("MALLOC FAILED");
        return NULL;
    }

    int req_blocks = req_len/MINALLOC;
    for(int page = 0; page<PAGES; page++){
        if(base_ptr[page] == NULL){
            base_ptr[page] = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        }

        int free_start_idx = get_free_start_idx(page, req_len, req_blocks);
        if(free_start_idx!=-1){

            for(int i = free_start_idx; i<(free_start_idx + req_blocks); i++){
                is_free[page][i] = 0;
            }
            alloc_blocks_len[page][free_start_idx] = req_blocks;
            return (char*)(base_ptr[page] + (free_start_idx*MINALLOC));
        }
    }
    
    return NULL;
}

// Merging adjacent free blocks is possible with is_free arr
// allocated pages are not freed and given back to OS until the program completely terminates
void dealloc(char *ptr){
    // printf("%d\n", ptr);
    int corresponding_page = -1, idx;
    for(int i = 0; i<PAGES; i++){
        idx = (int)(ptr - (char*)base_ptr[i])/MINALLOC;
        // printf("%d\n", idx);
        if(idx >= 0 && idx <= BLOCKS-1){
            corresponding_page = i;
            break;
        }
    }

    int len = alloc_blocks_len[corresponding_page][idx];
    alloc_blocks_len[corresponding_page][idx] = 0;

    for(int i = idx; i<(idx + len); i++){
        is_free[corresponding_page][i] = 1;
    }

}