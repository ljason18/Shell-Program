/* -----------------------------------------------------------------------------
FILE: shell.c

NAME:

DESCRIPTION: A SHELL SKELETON
-------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include "parse.h"   // local file include declarations for parse-related structs

enum BUILTIN_COMMANDS {NO_SUCH_BUILTIN=0, EXIT, JOBS};

/* -----------------------------------------------------------------------------
FUNCTION: buildPrompt()
DESCRIPTION:
-------------------------------------------------------------------------------*/
char *buildPrompt()
{
    return "gosh@HW05> ";
}

/* -----------------------------------------------------------------------------
FUNCTION: isBuild()
DESCRIPTION:
-------------------------------------------------------------------------------*/
int isBuiltInCommand(char *cmd)
{
    if(strncmp(cmd, "exit", strlen("exit")) == 0)
	{
        return EXIT;
  	}
    return NO_SUCH_BUILTIN;
}


/* -----------------------------------------------------------------------------
FUNCTION: main()
DESCRIPTION:
-------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    char *cmdLine;
    parseInfo *info; 		// info stores all the information returned by parser.
    struct commandType *com; 	// com stores command name and Arg list for one command.

    fprintf(stdout, "This is the SHELL version 0.1\n");

    while(1)
    {
    	// insert your code here
    	cmdLine = readline(buildPrompt()) ;
    	if(cmdLine == NULL)
        {
      		fprintf(stderr, "Unable to read command\n");
      		continue;
    	}
    	// insert your code about history and !x !-x here


    	// calls the parser
    	info = parse(cmdLine);
        if(info == NULL)
        {
      		free(cmdLine);
      		continue;
    	}

    	// prints the info struct
//    	print_info(info);

    	//com contains the info. of the command before the first "|"
    	com = &info->CommArray[0];
    	if((com == NULL) || (com->command == NULL))
    	{
      		free_info(info);
      		free(cmdLine);
      		continue;
    	}

    	//com->command tells the command name of com
    	if(isBuiltInCommand(com->command) == EXIT)
        {
      		exit(1);
    	}

        // insert your code here / commands etc.
        int pipe_fd[info->pipeNum][2];
        pid_t pid;
        char *cmd = com->command;
        char *cmd_args[com->VarNum + 1];

        for (int num = 0; num < com->VarNum; num++)
        {
            cmd_args[num] = com->VarList[num];
        }
        cmd_args[com->VarNum] = NULL;

        if (info->pipeNum > 0)
        {
            // Create pipes in a for loop
            for (int i = 0; i < info->pipeNum; i++)
            {
                if ((pipe(pipe_fd[i])) < 0)
                {
                    perror("pipe");
                    exit(-1);
                }
            }

            for (int pipe = 0; pipe < info->pipeNum + 1; pipe++)
            {
                if ((pid = fork()) < 0)
                {
                    perror("fork");
                    exit(-1);
                }
                else if (pid == 0)
                {
                    if (pipe != 0)
                    {
                        if (dup2(pipe_fd[pipe - 1][0], 0) < 0)
                        {
                            perror("dup2");
                            exit(EXIT_FAILURE);
                        }
                    }

                    if (pipe != info->pipeNum)
                    {
                        if (dup2(pipe_fd[pipe][1], 1) < 0)
                        {
                            perror("dup2");
                            exit(EXIT_FAILURE);
                        }
                    }

                    for (int i = 0; i < info->pipeNum; i++)
                    {
                        close(pipe_fd[i][0]);
                        close(pipe_fd[i][1]);
                    }
                    execvp(cmd, cmd_args);
                    perror("execvp");
                    exit(errno);
                }
            }

            for (int r = 0; r < info->pipeNum + 1; r++)
            {
                for (int c = 0; c < 2; c++)
                {
                    close(pipe_fd[r][c]);
                }
            }

            for (int w = 0; w < info->pipeNum + 1; w++)
            {
                if (wait(NULL) < 0)
                {
                    perror("wait");
                    exit(-1);
                }
            }
        }
        else
        {
            if ((pid = fork()) < 0)
            {
                perror("fork");
                exit(errno);
            }
            else if (pid == 0)
            {
                execvp(cmd, cmd_args);
                perror("execvp");
                exit(errno);
            }
            else if (pid > 0)
            {
                wait(NULL);
            }
        }

        free_info(info);

        free(cmdLine);
    }/* while(1) */

} // main
