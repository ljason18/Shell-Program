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
#include <fcntl.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"   // local file include declarations for parse-related structs

enum BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT, JOBS };
#define UNUSED(x) (void)(x)
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
    char *home = getenv("HOME");

    fprintf( stdout, "This is the SHELL version 0.1\n" ) ;
    int i = 0;
    while(1)
    {
    	// insert your code here
        dup2(0,0);
        dup2(1,1);
        if (i > 0)
        {
            free(cmdLine);
        }

        i++;
        cmdLine = readline( buildPrompt() ) ;
        if( cmdLine == NULL )
        {
            exit(0);
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
//        print_info( info );

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

        // insert your code here / commands etc.
/*
        int pipefd[2];
        pid_t pid;
        int in_fd = STDIN_FILENO;
        int out_fd = STDOUT_FILENO;
//        char *in[info->pipeNum + 1], *out[info->pipeNum + 1];

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

            if (info->boolInfile)
            {
//                printf("input\n");
                if ((in_fd = open(info->inFile, O_RDONLY)) < 0)
                {
                    perror("open");
                    exit(errno);
                }
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }

            if (info->boolOutfile)
            {
//                printf("output\n");
                if ((out_fd = open(info->outFile, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
                {
                    perror("open");
                    exit(errno);
                }
//                printf("dup\n");
                if (dup2(out_fd, STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
//                printf("close\n");
                if (close(out_fd) < 0)
                {
                    perror("close");
                    exit(errno);
                }
//                printf("end\n");

            }

            if (cmd_num == info->pipeNum)
            {
                out_fd = STDOUT_FILENO;
            }

//
            if (in_fd != STDIN_FILENO)
            {
                close(in_fd); // close input file descriptor before forking
            }
//            printf("pipe\n");
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
//                printf("child\n");
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
                else if (out_fd != STDOUT_FILENO)
                {
                    // redirect output to file
                    dup2(out_fd, STDOUT_FILENO);
                    close(out_fd);
                }
                execvp(args[0], args);
                perror(args[0]);
                exit(errno);
            }
            else
            {
//                printf("parent\n");
                // close write end of pipe
                close(pipefd[1]);
                if (in_fd == STDIN_FILENO)
                {
//                    close(in_fd);
                    in_fd = pipefd[0];
                }
                // input for next command
//                in_fd = pipefd[0];
                if (out_fd != STDOUT_FILENO)
                {
                    close(out_fd);
                }

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
//    printf("end\n");

    wait(NULL);
*/
        char *pipeline_commands[info->pipeNum + 1];
        char ***pipeline_args = malloc(sizeof(char**) * (info->pipeNum + 1));
        char *inputs[info->pipeNum + 1];
        char *outputs[info->pipeNum + 1];

        for (int i = 0; i < info->pipeNum + 1; i ++)
        {
            inputs[i] = NULL;
            outputs[i] = NULL;
        }
        for (int i = 0; i < info->pipeNum + 1; i ++)
        {
            com = &info->CommArray[i];
            if( (com == NULL)  || (com->command == NULL))
            {
                free_info(info);
                free(cmdLine);
                continue;
            }

            //com->command tells the command name of com
            char **args = malloc(sizeof(char*) * (com->VarNum + 1));
            pipeline_commands[i] = com->command;

            for(int k = 0; k < com->VarNum; k ++)
            {
                args[k] = com->VarList[k];
            }

            args[com->VarNum] = NULL;

            pipeline_args[i] = args;
            if (strlen(info->inFile) > 0)
            {
                inputs[i] = info->inFile;

            }
            if (strlen(info->outFile) > 0)
            {
                outputs[i] = info->outFile;
            }

        }
        int number_commands = sizeof(pipeline_commands) / sizeof(pipeline_commands[0]);

        if(strcmp(pipeline_commands[0], "cd") == 0)
        {
            char curr_home[500];
            strcpy(curr_home, home);

            if (strcmp(pipeline_args[0][1], "~") == 0)
            {
                if ((chdir(curr_home)) != 0)
                {
                    perror("cd");
                    exit(errno);
                }
            }
            else if (pipeline_args[0][1] != NULL)
            {
                if(pipeline_args[0][1][0] == '~')
                {
                    char new_arg[500];
                    for (int i = 1; i < strlen(pipeline_args[0][1]); i ++)
                    {
                        new_arg[i - 1] = pipeline_args[0][1][i];
                    }
                    strcat(curr_home, new_arg);

                    if ((chdir(curr_home)) != 0)
                    {
                        perror("cd");
                        exit(errno);
                    }
                }
                else
                {
                    if((chdir(pipeline_args[0][1])) != 0)
                    {
                        perror("cd");
                        exit(errno);
                    }
                }
            }
            continue;
        }

        // This is a test
        int pipes = number_commands - 1;
        int pipe_fd[pipes][2];
        UNUSED(pipe_fd);
        for(int i = 0; i < pipes; i++)
        {
            if((pipe(pipe_fd[i])) < 0)
            {
                perror("pipe");
                exit(errno);
            }
        }
        pid_t pid;

        for(int k = 0; k < number_commands; k ++)
        {
            pid = fork();

            if (pid == 0)
            {
                // Test
                if (inputs[k] != NULL)
                {
                    int fd = open(inputs[k], O_RDONLY);
                    dup2(fd, 0);
                }

                if (outputs[k] != NULL)
                {
                    int fd = open(outputs[k], O_CREAT | O_WRONLY | O_TRUNC, 0644);
                    dup2(fd, 1);
                }
                // End Test

                if (k != 0)
                {
                    if (dup2(pipe_fd[k - 1][0], 0) < 0)
                    {
                        perror("dup2");
                        exit(errno);
                    }
                }
                if (k != pipes)
                {
                    if (dup2(pipe_fd[k][1], 1) < 0)
                    {
                        perror("dup2");
                        exit(errno);
                    }
                }

                for (int y = 0; y < pipes; y ++)
                {
                    close(pipe_fd[y][0]);
                    close(pipe_fd[y][1]);
                }

                execvp(pipeline_commands[k], pipeline_args[k]);
                perror("execvp");
                exit(errno);
            }
            else if (pid < 0)
            {
                perror("fork");
                exit(errno);
            }
        }
        for (int i = 0; i < number_commands; i ++)
        {
            close(pipe_fd[i][0]);
            close(pipe_fd[i][1]);
        }

        for (int i = 0; i < number_commands; i ++)
        {
            if (wait(NULL) < 0)
            {
                perror("wait");
                exit(errno);
            }
        }
        wait(NULL);
        // End of test
/*        if (cmdLine != NULL)
        {
            free(cmdLine);
            }*/
        free_info(info);

  }/* while(1) */
} // main

void execute_pipeline(char *commands[], char **commands_args[], int number_commands)
{
    printf("Into function\n");
    printf("Command 1: %s\n", commands[0]);
    printf("Num : %d\n", number_commands);
    int pipes = number_commands - 1;
    int pipe_fd[pipes][2];
    UNUSED(pipe_fd);

    // creates pipes
    for (int i = 0; i < pipes; i ++)
    {
        if((pipe(pipe_fd[i])) < 0)
        {
            perror("pipe");
            exit(errno);
        }
    }
    printf("Created pipes\n");
    // parent forks children
    int pid;
    for (int i = 0; i < number_commands; i ++)
    {
        pid = fork();
        if (pid == 0)
        {
            printf("This");
            if (i != 0)
            {
                if (dup2(pipe_fd[i - 1][0], 0) < 0)
                {
                    perror("dup2");
                    exit(errno);
                }
            }
            printf("here?");
            if (i != pipes)
            {
                if (dup2(pipe_fd[i][1], 1) < 0)
                {
                    perror("dup2");
                    exit(errno);
                }
            }

            for (int y = 0; y < pipes; y ++)
            {
                close(pipe_fd[y][0]);
                close(pipe_fd[y][1]);
            }

            execvp(commands[i], commands_args[i]);
            perror("execvp");
            exit(errno);

        }
        else if (pid < 0)
        {
            perror("fork");
            exit(errno);
        }
    }

    for (int i = 0; i < number_commands; i ++)
    {
        close(pipe_fd[i][0]);
        close(pipe_fd[i][1]);
    }

    for (int i = 0; i < number_commands; i ++)
    {
        if (wait(NULL) < 0)
        {
            perror("wait");
            exit(errno);
        }
    }

} // execute_pipeline
