#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_BG_PROCS 64

int bg_procs_idx = 0;
int *bg_procs;

void add_bg_procs(int pid){
	if(bg_procs_idx == MAX_BG_PROCS){
		printf("Reached Maximum limit of Background processes, Can't handle any more\n");
	}
	bg_procs[bg_procs_idx] = pid;
	bg_procs_idx++;
}

void reap_bg_procs(){
	for(int i = 0; i<=bg_procs_idx; i++){
		int pid = waitpid(bg_procs[i],NULL,WNOHANG);
		if(pid > 0){
			printf("Shell: Background process finished\n");
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
	}else{
		int pid = fork();

		if( pid == 0){
			// child process execution
			// When you click ctrl + c it sends a SIGTERM signal to shell and child processes as they belong to the same parent group
			// Changed parent group of background child processes sothat only shell process and foreground (currently running) process is terminated
			if(is_background == 1){
				setpgid(getpid(), 0);
			}
			
			printf("child process - id: %d pgid: %d\n",getpid(), getpgid(getpid()));
			if(execvp(tokens[0], tokens) == -1){
				printf("Wrong Command");
			}
		}else{
			// parent process execution
			printf("Main process - id: %d pgid: %d\n",getpid(), getpgid(getpid()));
			if(is_background == 1){
				add_bg_procs(pid);
			}else{
				waitpid(pid,NULL,0);
			}
		}
	} 
}

void handle_sigint(int i, siginfo_t* info, void* ctxt){
	printf("I am running\n");
	exit(0);
}

int main(int argc, char* argv[]) {

	struct sigaction sa = {0};
	sa.sa_sigaction = &handle_sigint;
	sigaction(SIGINT, &sa, NULL);

	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              

	bg_procs = (int*)malloc(sizeof(int));
	fflush(stdout);
	int is_exited = 0;
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

		if(strcmp(tokens[0], "exit") == 0){
			for(int i = 0; i<bg_procs_idx; i++){
				kill(bg_procs[i], SIGKILL);
				int status = 0;
				int reaped_pid = waitpid(bg_procs[i],&status,0);
				if(WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL){
					printf("%d is_reaped %d\n", bg_procs[i], reaped_pid);
				}else{
					printf("Unexpected Normal Exit\n");
				}
			}
			is_exited = 1;
		}else{
			start_executing(tokens, is_background);
		}

       
		// Freeing the allocated memory	
		for(int i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

		if(is_exited == 0){
			reap_bg_procs();
		}else{
			free(bg_procs);
			break;
		}
	}

	if(is_exited == 0){
		while(bg_procs_idx > 0){
			reap_bg_procs();
			sleep(1);
		}
	}
	return 0;
}
