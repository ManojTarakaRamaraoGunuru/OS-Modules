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

int min(int a, int b){
	if(a<b)return a;
	return b;
}

int get_file_handle_idx(){
	for(int i = 0; i<MAX_OPEN_FILES; i++){
		if(file_handle_array[i].inode_number == -1){
			return i;
		}
	}
	return -1;
}

int simplefs_create(char *filename){
    /*
	    Create file with name `filename` from disk
	*/
	
	
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

	struct inode_t* inode_ptr = (struct inode_t*)malloc(sizeof(struct inode_t));
	simplefs_readInode(inode_num, inode_ptr);

	for(int i = 0; i<MAX_FILE_SIZE; i++){
		int data_block_num = inode_ptr->direct_blocks[i];
		if(data_block_num != -1){
			simplefs_freeDataBlock(data_block_num);
		}
	}

	simplefs_freeInode(inode_num);
}

int simplefs_open(char *filename){
    /*
	    open file with name `filename`
	*/
	int inode_num = get_inode_num(filename);
	if(inode_num == -1)return -1;

	int file_handle = get_file_handle_idx();
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
	int inode_num = file_handle_array[file_handle].inode_number;
	int curr_offset = file_handle_array[file_handle].offset;

	struct inode_t* inode_ptr = (struct inode_t*)malloc(sizeof(struct inode_t));
	simplefs_readInode(inode_num, inode_ptr);

	if(curr_offset + nbytes > inode_ptr->file_size){
		free(inode_ptr);
		return -1;
	}

	int copied_bytes = 0;
	int initial_offset = curr_offset;
	while(copied_bytes<nbytes){

		int roffset = curr_offset%BLOCKSIZE;
		int rblock_idx = curr_offset/BLOCKSIZE;

		size_t buf_size = min(BLOCKSIZE - roffset, nbytes  - copied_bytes);
		char*temp_buf = (char*)malloc(buf_size*sizeof(char));
		simplefs_readDataBlock(inode_ptr->direct_blocks[rblock_idx], temp_buf);
		
		// printf("%d %d %s\n",copied_bytes, buf_size, temp_buf);

		memcpy(buf + copied_bytes, temp_buf, buf_size);
		free(temp_buf);

		curr_offset += buf_size;
		copied_bytes += buf_size;
	}
	free(inode_ptr);
    return 0;
}


int simplefs_write(int file_handle, char *buf, int nbytes){
    /*
	    write `nbytes` of data from `buf` to file pointed by `file_handle` starting at current offset
	*/

	int inode_num = file_handle_array[file_handle].inode_number;
	int curr_offset = file_handle_array[file_handle].offset;


	struct inode_t* inode_ptr = (struct inode_t*)malloc(sizeof(struct inode_t));
	simplefs_readInode(inode_num, inode_ptr);

	if(curr_offset + nbytes > MAX_FILE_SIZE*BLOCKSIZE){
		free(inode_ptr);
		return -1;
	}

	// int initial_fill_size = curr_offset;
	// printf("initial_offset: %d ", initial_fill_size);
	int copied_bytes = 0;
	
	while(copied_bytes<nbytes){
		int filled_wblock = inode_ptr->file_size % BLOCKSIZE;
		int wblock_idx = inode_ptr->file_size/BLOCKSIZE;
		
		if(filled_wblock == 0){
			int idx = simplefs_allocDataBlock();
			if(idx == -1)return -1;
			inode_ptr->direct_blocks[wblock_idx] = idx;
		}
		size_t buf_size = min(BLOCKSIZE - filled_wblock, nbytes - copied_bytes);

		char* temp_buf = (char*)malloc(buf_size*sizeof(char));
		memcpy(temp_buf, buf + copied_bytes, buf_size);
		// printf("%d %d %s\n",buf_size, inode_ptr->file_size temp_buf);
		simplefs_writeDataBlock(inode_ptr->direct_blocks[wblock_idx], temp_buf);
		free(temp_buf);

		copied_bytes += buf_size;
		inode_ptr->file_size += buf_size;
	}
	// printf("\n");
	simplefs_writeInode(inode_num, inode_ptr);
	free(inode_ptr);

    return 0;
}


int simplefs_seek(int file_handle, int nseek){
    /*
	   increase `file_handle` offset by `nseek`
	*/
	// printf("File Handle %d\n", file_handle);

	int curr_offset = file_handle_array[file_handle].offset;
	int inode_num = file_handle_array[file_handle].inode_number;
	struct inode_t* inode_ptr = (struct inode_t*)malloc(sizeof(struct inode_t));
	simplefs_readInode(inode_num, inode_ptr);


	if(curr_offset + nseek<0 || curr_offset + nseek > inode_ptr->file_size){
		free(inode_ptr);
		return -1;
	}

	file_handle_array[file_handle].offset += nseek;
	free(inode_ptr);
    return 0;
}