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

enum BUILTIN_COMMANDS {NO_SUCH_BUILTIN=0, EXIT, JOBS};
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
    free(buf);
    return prompt;
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
    char *home = getenv("HOME");

    fprintf( stdout, "This is the SHELL version 0.1\n" ) ;
    while(1)
    {
    	// insert your code here
        cmdLine = readline(buildPrompt());
        if(cmdLine == NULL)
        {
            fprintf(stderr, "Unable to read command\n");
            continue;
        }
        // insert your code about history and !x !-x here
        if (strcmp(cmdLine, "help") == 0)
        {
            printf("command\n");
            printf("command arg1 arg2 ... arg\n");
            printf("command arg1 arg2 < infile.txt\n");
            printf("command arg1 arg2 > outfile.txt\n");
            printf("command < infile.txt > outfile.txt\n");
            printf("command1 | command2\n");
            printf("command1 | command2 | ...  | command8\n");
            printf("command1 < infile.txt | command2 | ...  | command8 > outfile.txt\n");
            printf("cd directory\n");
            printf("cd ~/directory\n");
            printf("cd ~user/directory\n");
            printf("pwd\n");
            printf("exit\n");
            printf("help\n");
            continue;
        }

    	// calls the parser
    	info = parse(cmdLine);
        if(info == NULL)
        {
      		free(cmdLine);
      		continue;
    	}

    	// prints the info struct
//        print_info( info );

    	//com contains the info. of the command before the first "|"
    	com = &info->CommArray[0];
    	if ((com == NULL) || (com->command == NULL))
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
        char *pipeline_commands[info->pipeNum + 1];
        char ***pipeline_args = malloc(sizeof(char**) * (info->pipeNum + 1));
        char *inputs[info->pipeNum + 1];
        char *outputs[info->pipeNum + 1];
        char *appends[info->pipeNum + 1];

        for (int i = 0; i < info->pipeNum + 1; i++)
        {
            inputs[i] = NULL;
            outputs[i] = NULL;
        }
        for (int i = 0; i < info->pipeNum + 1; i++)
        {
            com = &info->CommArray[i];
            if((com == NULL) || (com->command == NULL))
            {
                free_info(info);
                free(cmdLine);
                continue;
            }
/*
            if(isBuiltInCommand(com->command) == EXIT)
            {
                exit(1);
            }
*/
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
            if (strlen(info->outFile) > 0 && info->boolOutfile == 2)
            {
                appends[i] = info->outFile;
            }
            else if (strlen(info->outFile) > 0)
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
                    continue;
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
                        continue;
                    }
                }
                else
                {
                    if((chdir(pipeline_args[0][1])) != 0)
                    {
                        perror("cd");
                        continue;
                    }
                }
            }
            continue;
        }

        // This is a test
        int pipes = info->pipeNum;
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
                    int in_fd;
                    if ((in_fd = open(inputs[k], O_RDONLY)) < 0)
                    {
                        perror("open");
                        exit(errno);
                    }
                    dup2(in_fd, 0);
                }

                if (outputs[k] != NULL)
                {
                    int out_fd;
                    if ((out_fd = open(outputs[k], O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0)
                    {
                        perror("open");
                        exit(errno);
                    }
                    dup2(out_fd, 1);
                }

                if (appends[k] != NULL)
                {
                    int app_fd;
                    if ((app_fd = open(appends[k], O_WRONLY | O_APPEND)) < 0)
                    {
                        perror("open");
                        exit(errno);
                    }
                    dup2(app_fd, 1);
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
        for (int i = 0; i < pipes; i ++)
        {
            close(pipe_fd[i][0]);
            close(pipe_fd[i][1]);
        }

        for (int i = 0; i < pipes; i ++)
        {
            if (wait(NULL) < 0)
            {
                perror("wait");
                exit(errno);
            }
        }
        wait(NULL);
        // End of test
        free(cmdLine);
        free(pipeline_args);
        free_info(info);
  }/* while(1) */
} // main
