#include <stdio.h>		// printf(), fgets(), stdin, fprintf(), stderr
#include <string.h>		// strlen(), strtok(), strcmp()
#include <sys/wait.h>	// wait()
#include <unistd.h>		// fork(), exec()

#include <stdlib.h>
int main()
{
	int n=0;
	char *a;
    char command_line[100];	// This is the command line argument
    char* command[100];		// command is stored
    printf("SHELL\nType exit to exit (Note: It is case sensitive)\n");
// An infinite loop is set using while which can be exited by either typing exit or pressing Ctrl+D
	while(1)
	{
		printf("Monica'S Shell>	");

//fgets() is false if Ctrl + D is pressed otherwuse it is true and it stores the command entered into the command_line
		if(!fgets(command_line, 100, stdin))
		{
			printf("\n");
			break;
		}

//The enter pressed after command is converted into NULL
    	int len = strlen(command_line);
    	if (command_line [len - 1] == '\n')
        {	command_line [len - 1] = '\0';	}

//IT parses the command recieved, command_line is parsed and stored in command
	    a = strtok(command_line," ");
    	while(a!=NULL)
    	{
        	command[n]=a;
        	a = strtok(NULL," ");
        	n++;
    	}
    	command[n] = NULL;		// last value is set to NULL to indicate the end of the command

//This condition exits from loop by typing exit
		if(strcmp(command_line, "exit")==0)
		{	break;	}

		int pid= fork();		//Parent process is duplicated using fork()

		switch(pid)
		{
			case 0 :			// child process is given a process using execvp()
				execvp(command[0],command);		//First argument sets path and next argument is for value
        		fprintf(stderr, "(ERROR - Child Process Could Not Execute This Command\n"); //This statement prints if there is an error
				break;
			case -1 :
				printf("ERROR - FAILED TO FORK\n");
				break;
			default :
        		wait(NULL);			// Parent process waits for child process
        		break;
		}

	}
}
