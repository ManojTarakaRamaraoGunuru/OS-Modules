#include "simplefs-ops.h"
extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files

int get_inode_num(char* filename){
	/* retunrns ind*/
	struct superblock_t* superblock = (struct superblock_t*)malloc(sizeof(struct superblock_t));
	int inode_num = -1;
	for(int i = 0; i<NUM_INODES; i++){
		if(superblock->inode_freelist[i] == INODE_IN_USE){
			struct inode_t* inode_ptr = (struct inode_t*)malloc(sizeof(struct inode_t));
			simplefs_readInode(i, inode_ptr);
			if(strcmp(inode_ptr->name, filename) == 0){
				inode_num = i;
				free(inode_ptr);
				break;
			}
			free(inode_ptr);
		}
	}
	free(superblock);
	return inode_num;
}

int simplefs_create(char *filename){
    /*
	    Create file with name `filename` from disk
	*/
	
	
	if(get_inode_num(filename) != -1){ 
		return -1;
	}

	int inode_num = simplefs_allocInode();
	if(inode_num == -1){
		return -1;
	}

	struct inode_t* inode_ptr = (struct inode_t*)malloc(sizeof(struct inode_t));
	simplefs_readInode(inode_num, inode_ptr);
	memcpy(inode_ptr->name, filename, sizeof(filename));
	inode_ptr->status = INODE_IN_USE;
	simplefs_writeInode(inode_num, inode_ptr);
	free(inode_ptr);

    return inode_num;
}


void simplefs_delete(char *filename){
    /*
	    delete file with name `filename` from disk
	*/
	int inode_num = get_inode_num(filename);
	if(inode_num == -1)return;

	simplefs_freeInode(inode_num);
}

int simplefs_open(char *filename){
    /*
	    open file with name `filename`
	*/
    return -1;
}

void simplefs_close(int file_handle){
    /*
	    close file pointed by `file_handle`
	*/

}

int simplefs_read(int file_handle, char *buf, int nbytes){
    /*
	    read `nbytes` of data into `buf` from file pointed by `file_handle` starting at current offset
	*/
    return -1;
}


int simplefs_write(int file_handle, char *buf, int nbytes){
    /*
	    write `nbytes` of data from `buf` to file pointed by `file_handle` starting at current offset
	*/
    return -1;
}


int simplefs_seek(int file_handle, int nseek){
    /*
	   increase `file_handle` offset by `nseek`
	*/
    return -1;
}