#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_BG_PROCS 64

int bg_procs_idx = 0;
int bg_procs[MAX_BG_PROCS];

void add_bg_procs(int pid){
	if(bg_procs_idx == MAX_BG_PROCS){
		printf("Reached Maximum limit of Background processes, Can't handle any more");
	}
	bg_procs[bg_procs_idx] = pid;
	bg_procs_idx++;
}

void reap_bg_procs(){
	for(int i = 0; i<=bg_procs_idx; i++){
		int pid = waitpid(bg_procs[i],NULL,WNOHANG);
		if(pid > 0){
			printf("Shell: Background process finished");
			bg_procs[i] = bg_procs[bg_procs_idx];
			bg_procs_idx--;
		}
	}
}

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
		token[tokenIndex] = '\0';
		if (tokenIndex != 0){
			tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
			strcpy(tokens[tokenNo++], token);
			tokenIndex = 0; 
		}
    } else {
        token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void start_executing(char** tokens, int is_background){

	if(strcmp(tokens[0], "cd")==0){
		if(tokens[1] == NULL){
			chdir("/home/kali");
		}
		else if(chdir(tokens[1])==-1){
			printf("Unable to change the directory with the gven path");
		}
		return;
	}
	int pid = fork();

	if( pid == 0){
		// child process execution
		if(execvp(tokens[0], tokens) == -1){
			printf("Wrong Command");
		}
	}else{
		// parent process execution
		if(is_background == 1){
			add_bg_procs(pid);
		}else{
			waitpid(pid,NULL,0);
		}
	} 
}

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              


	while(1) {			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		char** temp = tokens;
		int is_background = 0;
		for(int i = 0; temp[i]!=NULL; i++){
			if(strcmp(temp[i], "&") == 0) {
				is_background = 1;
				tokens[i] = NULL;
			}
		}


		start_executing(tokens, is_background);
       
		// Freeing the allocated memory	
		for(int i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

		reap_bg_procs();
	}
	while(bg_procs_idx > 0){
		reap_bg_procs();
		sleep(1);
	}
	return 0;
}
