#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <linux/limits.h>
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define LSH_RL_BUFSIZE 1024
#define DELIM " \t\r\a"

// global variable indicating if we should wait for command(s) to finish
int waitc = 1;
void alloc_err()
{
  fprintf(stderr, "%s\n", "lsh: błąd alokacji");
  exit(EXIT_FAILURE);
}
// handles our funny signals
void signal_handler(int no) {

	// if signal is SIGCHLD, let's wait for it
	if (no == 17) {
		waitpid(-1, NULL, WNOHANG);

	// we want to ignore SIGINT
	} else if (no == 2) {}
}
char *readLine()
{
  int buffer_size = LSH_RL_BUFSIZE;
  int i = 0;
  char *buffer = malloc(sizeof(char)*buffer_size);
  char c;
  if(!buffer)
  {
    alloc_err();
  }
  while(1){
    c = getchar();
    //EOF = ctrl+D
    if(c==EOF)
		{
			printf("\n");
			exit(0);
		}
    if(c == '\n')
    {
      buffer[i] = '\0';
      return buffer;
    }
    else
    {
      buffer[i]=c;
    }
    i++;

    if(i>buffer_size)
    {
      buffer_size+=LSH_RL_BUFSIZE;
      buffer = realloc(buffer, buffer_size);
      if(!buffer)
      {
        alloc_err();
      }
    }
  }
	return buffer;
}
// printf our prompt, reads from console to given array of strings, return number of words
int promptRead (char* outputarray[]) {
	char input[PATH_MAX];
	//outputarray
	char* word;

	// will be used by strtok as separator

	// currect directory
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));

	// print
	printf("[\e[38;5;160mLSH \e[38;5;33m%s\e[0m] ", cwd);
/*
	// read
	int c = getchar();
	if(c==EOF)exit(0);
	fgets(input, PATH_MAX, stdin);
	//strcpy(inputt, c);
	//strcat(inputt, input);
	//printf("%s\n",inputt );
	// ignore newline chars

	char *x = (char*)malloc(sizeof(char) * (strlen(input) + 1));
	x[0] = c;
	for(int i = 0; i<strlen(input); i++)
	 	x[i+1]=input[i];

	//sprintf(inputt,"%s%s",c,input);
	printf("x%s\n%c\n%s\n",x,c,input );
	// will hold our return value
	*/
	char *x = readLine();
	int i = 0;

	// let's split it
	word = strtok(x, LSH_TOK_DELIM);
	while (word != NULL) {
		outputarray[i] = calloc(strlen(word) + 1, sizeof(char));
		strcpy(outputarray[i], word);
		i++;
		word = strtok(NULL, LSH_TOK_DELIM);
	}
	free(x);
	return i;
}

// sets fds to files we specify
// modes:
// 0 - read
// 1 - write
// -1 - none
// 2 - all
void addRedirects(int size, char* commands[][size], int index, int mode) {

	int commandLength = 0;
	while (commands[index][commandLength] != NULL) commandLength++;

	for(int j = 0; j < commandLength; j++) {

		if(commands[index][j][0] == '>' && (mode == 1 || mode == 2)) {

			char path[PATH_MAX];
			for(int k = 1; k < strlen(commands[index][j]); k++) {
				path[k-1] = commands[index][j][k];
			}

			int file;
			if ((file = open(path, O_RDWR | O_CREAT | O_APPEND)) == -1) {
				perror("> error");
				_exit(1);
			}
			//dup2(file,1) - kopiowanie z stdout do file
			if (dup2(file, 1) == -1) {
				perror("dup2 error when tryging to redirect to file");
			}

			commands[index][j] = NULL;

		} else if (commands[index][j][0] == '2' && commands[index][j][1] == '>') {

			char path[PATH_MAX];
			for(int k = 2; k < strlen(commands[index][j]); k++) {
				path[k-2] = commands[index][j][k];
			}

			int file;
			if ((file = open(path, O_RDWR | O_CREAT | O_APPEND)) == -1) {
				perror("2> error");
				_exit(1);
			}
			if (dup2(file, 2) == -1) {
				perror("dup2 error when tryging to redirect to file");
			}

			commands[index][j] = NULL;

		} else if (commands[index][j][0] == '<' && (mode == 0 || mode == 2)) {

			char path[PATH_MAX];
			for(int k = 1; k < strlen(commands[index][j]); k++) {
				path[k-1] = commands[index][j][k];
			}

			int file;
			if ((file = open(path, O_RDWR | O_CREAT | O_APPEND)) == -1) {
				perror("> error");
				_exit(1);
			}
			//dup2(file,0) - kopiowanie z file do stdin
			if (dup2(file, 0) == -1) {
				perror("dup2 error when tryging to redirect to file");
			}

			commands[index][j] = NULL;

		}
	}
}

// it's called pipe although it handles all command execs
void pipeThrough(char* pipedCommands[], int size) {

	// stores index for last | symbol
	int foundIndex = size;
	int commandsCount = 0;
	char* commands[size][size];
	for (int i = size - 1; i >= 0; i--) {

		if (pipedCommands[i][0] == '|' || i == 0) {

			int k = 0;

			// idziemy od końca,
			int j = i + 1;
			if (i == 0) {
				j = 0;
			}
			// split each command into words
			for (; j < foundIndex; j++) {
				// commands is allocated and written to and yet we don't ever free it, because the process dies eventually, so, whatever
				commands[commandsCount][k] = calloc(strlen(pipedCommands[j]) + 1, sizeof(char));
				strcpy(commands[commandsCount][k], pipedCommands[j]);
				k++;
			}
			commands[commandsCount][k] = NULL;

			commandsCount++;
			foundIndex = i;
		}
	}

	// create fds shared between forks later, we need one less than processes we want to connect
	int fd[commandsCount - 1][2];
	for (int i = 0; i < commandsCount - 1; i++) {
		if (pipe(fd[i]) == -1) {
			perror("pipe error");
		}
	}

	// we will store processes pids to wait for them later
	int pids[commandsCount];
	int status, wpid;

	for (int i = 0; i < commandsCount; i++) {
		if ((pids[i] = fork()) == 0) {

			if (commandsCount > 1) {
				if (i == 0) {
					// case for the LAST command in pipes
					//output poprzedniej komendy to input tej 
					if (dup2(fd[i][0], 0) == -1) {
						perror("dup2 error on i=0");
					}
					//ewentualne przekierowanie write do pliku
					addRedirects(size, commands, i, 1);

				} else if (i != commandsCount - 1) {
					// case for all the MIDDLE commands in pipes
					//wynik tego to write dla kolejnej komendy!
					if (dup2(fd[i-1][1], 1) == -1) {
						perror("dup2 error");
					}
					//wejście tego to wynik poprzedniej komendy, i+1 -> i
					if (dup2(fd[i][0], 0) == -1) {
						perror("dup2 error");
					}

					addRedirects(size, commands, i, -1);

				} else {
					// case for the FRIST command in pipes


					//wynik tego to write do kolejnej komendy!! EUREKA 1:00!!!!!



					if (dup2(fd[i-1][1], 1) == -1) {
						perror("dup2 error on i=commands-1");
					}
						//ewentualne przejęcie stdin z pliku
					addRedirects(size, commands, i, 0);
				}
			} else {
				// if we have only one command, we add all redirects to it
				addRedirects(size, commands, 0, 2);
			}

			// we close all unused fds - important!
			for (int j = 0; j < commandsCount - 1; j++) {
				close(fd[j][0]);
				close(fd[j][1]);
			}

			// we execute the command - process is replaced by it and dies off eventually, freeing resources
			execvp(commands[i][0], commands[i]);
			// if we get to this part, execvp failed
			perror("execvp error");
			_exit(1);

		}
	}

	// close fds in parent
	for (int j = 0; j < commandsCount - 1; j++) {
		close(fd[j][0]);
		close(fd[j][1]);
	}

	// if we decided to wait, wait for all
	if(waitc) {
		for(int i = commandsCount - 1; i > -1; i--) {
			if(pids[i] > 0) {
				int status;
				waitpid(pids[i], &status, 0);
			} else {
				// if pid isn't greater than 0 then... what
			}
		}
	}
	exit(0);
}

int main() {
	char* words[PATH_MAX];

	signal(SIGCHLD, signal_handler);
	signal(SIGINT, signal_handler);

	while(1) {
		int i = promptRead(words);

		if (i == 0) {
			 continue;
		}

		int status, wpid;

		// cd and exit are handled in main process
		if (strcmp(words[0],"cd") == 0) {
			chdir(words[1]);
			continue;
		} else if (strcmp(words[0],"exit")==0 || words[0][0]==EOF) {
			kill(0, 9);
			exit(0);
		}

		// waitc is handled in main process
		if (words[i-1][0] == '&' && words[i-1][1] == '\0') {
			waitc = 0;
			i--;
		} else {
			waitc = 1;
		}

		//let's let children catch SIGINT to exit
		signal(SIGINT, SIG_DFL);
		signal(SIGCHLD, signal_handler);
		// let's fork and leave all the commands handling to fork
		int child = fork();

		if (child == 0) {
			pipeThrough(words, i);
		} else {
			signal(SIGINT, signal_handler);
			if (waitc) {
				wpid = waitpid(child, &status, 0);
			}
		}

		// let's free memory before allocating it again
		while (0 < --i) free(words[i]);
	}
	return 0;
}
