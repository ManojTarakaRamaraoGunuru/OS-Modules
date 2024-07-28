#include "simplefs-ops.h"
extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files

int get_inode_num(char* filename){
	/* returns inode number of the file*/
	int inode_num = -1;
	for(int i = 0; i<NUM_INODES; i++){
		
		struct inode_t* inode_ptr = (struct inode_t*)malloc(sizeof(struct inode_t));
		simplefs_readInode(i, inode_ptr);
		if(strcmp(inode_ptr->name, filename) == 0){
			inode_num = i;
			free(inode_ptr);
			break;
		}
		free(inode_ptr);
		
	}
	return inode_num;
}

int get_file_handle_idx(){
	for(int i = 0; i<MAX_OPEN_FILES; i++){
		if(file_handle_array[i].inode_num == -1){
			return i;
		}
	}
	return -1;
}

int simplefs_create(char *filename){
    /*
	    Create file with name `filename` from disk
	*/
	
	
	// if(get_inode_num(filename) != -1){ 
	// 	return -1;
	// }

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
	int inode_num = get_inode_num(filename);
	if(inode_num == -1)return -1;

	int file_handle = get_file_handle_idx()
	if(file_handle == -1)return -1;

	file_handle_array[file_handle].inode_number = inode_num;
	file_handle_array[file_handle].offset = 0;
    return file_handle;
}

void simplefs_close(int file_handle){
    /*
	    close file pointed by `file_handle`
	*/
	file_handle_array[file_handle].inode_number = -1;
	file_handle_array[file_handle].offset = 0;
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