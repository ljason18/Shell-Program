/* -----------------------------------------------------------------------------
FILE: shell.c

NAME:

DESCRIPTION: A SHELL SKELETON
-------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"   // local file include declarations for parse-related structs

enum BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT, JOBS };

/* -----------------------------------------------------------------------------
FUNCTION: buildPrompt()
DESCRIPTION:
-------------------------------------------------------------------------------*/
char * buildPrompt()
{
    char *buf = (char *) malloc(256);
    char *cwd;
    char *prompt = (char *) malloc(256);
    strcpy(prompt, "gosh@");
    realpath(".", buf);
    cwd = strrchr(buf, '/');
    strcat(prompt, &cwd[1]);
    strcat(prompt, "> ");
  return prompt;
}

/* -----------------------------------------------------------------------------
FUNCTION: isBuild()
DESCRIPTION:
-------------------------------------------------------------------------------*/
int isBuiltInCommand( char * cmd )
{
  if( strncmp(cmd, "exit", strlen( "exit" ) ) == 0 )
	{
	return EXIT;
  	}
  return NO_SUCH_BUILTIN;
}


/* -----------------------------------------------------------------------------
FUNCTION: main()
DESCRIPTION:
-------------------------------------------------------------------------------*/
int main( int argc, char **argv )
{
  char * cmdLine;
  parseInfo *info; 		// info stores all the information returned by parser.
  struct commandType *com; 	// com stores command name and Arg list for one command.

  fprintf( stdout, "This is the SHELL version 0.1\n" ) ;

  while(1)
  {
    	// insert your code here
    	cmdLine = readline( buildPrompt() ) ;
    	if( cmdLine == NULL )
        {
      		fprintf(stderr, "Unable to read command\n");
      		continue;
    	}
    	// insert your code about history and !x !-x here


    	// calls the parser
    	info = parse( cmdLine );
        if( info == NULL )
        {
      		free(cmdLine);
      		continue;
    	}

    	// prints the info struct
    	print_info( info );
/*
    	//com contains the info. of the command before the first "|"
    	com = &info->CommArray[0];
    	if( (com == NULL)  || (com->command == NULL))
    	{
      		free_info(info);
      		free(cmdLine);
      		continue;
    	}

    	//com->command tells the command name of com
    	if( isBuiltInCommand( com->command ) == EXIT )
        {
      		exit(1);
    	}
*/
        // insert your code here / commands etc.
        int pipefd[2];
        pid_t pid;
        int in_fd = STDIN_FILENO;

        for (int cmd_num = 0; cmd_num < info->pipeNum + 1; cmd_num++)
        {
            com = &info->CommArray[cmd_num];
            if( (com == NULL)  || (com->command == NULL))
            {
                free_info(info);
                free(cmdLine);
                continue;
            }

            //com->command tells the command name of com
            if( isBuiltInCommand( com->command ) == EXIT )
            {
                exit(1);
            }
            char *args[com->VarNum + 1];
            for (int num = 0; num < com->VarNum; num++)
            {
                args[num] = com->VarList[num];
            }
            args[com->VarNum] = NULL;

            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            if ((pid = fork()) == -1)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                // close read end of pipe
                close(pipefd[0]);
                if (cmd_num != 0)
                {
                    // redirect input from previous command
                    dup2(in_fd, STDIN_FILENO);
                    close(in_fd);
                }
                if (cmd_num != info->pipeNum)
                {
                    // redirect output to next command
                    dup2(pipefd[1], STDOUT_FILENO);
                }
                execvp(args[0], args);
                perror(args[0]);
                exit(EXIT_FAILURE);
            }
            else
            {
                // close write end of pipe
                close(pipefd[1]);
                if (in_fd != STDIN_FILENO)
                {
                    close(in_fd);
                }
                // input for next command
                in_fd = pipefd[0];
            }
        }
    for (int child = 0; child < info->pipeNum; child++)
    {
        // wait for child processes to finish
        if (wait(NULL) < 0)
        {
            perror("wait");
            exit(errno);
        }
    }

    wait(NULL);

  free_info(info);

  free(cmdLine);

  }/* while(1) */



} // main
