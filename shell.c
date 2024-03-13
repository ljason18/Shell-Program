/* -----------------------------------------------------------------------------
   FILE: shell.c

   NAME: Jason Lin

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

enum BUILTIN_COMMANDS {NO_SUCH_BUILTIN=0, EXIT, HELP, CD, JOBS};
extern int help();
extern int cd(struct commandType *com, char *home);
extern int execute_pipeline(parseInfo *info, struct commandType *com, char *cmdLine);

#define UNUSED(x) (void)(x)
#define TRUE 1
#define SUCCESS 0
#define ERROR -1
#define READ_END 0
#define WRITE_END 1

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
    if (strcmp(cmd, "exit") == SUCCESS)
    {
        return EXIT;
    }

    if (strcmp(cmd, "help") == SUCCESS)
    {
        return HELP;
    }

    if (strcmp(cmd, "cd") == SUCCESS)
    {
        return CD;
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
    while (TRUE)
    {
    	// Insert your code here
        cmdLine = readline(buildPrompt());
        if (cmdLine == NULL)
        {
            fprintf(stderr, "Unable to read command\n");
            continue;
        }
        // Insert your code about history and !x !-x here

    	// Calls the parser
    	info = parse(cmdLine);
        if (info == NULL)
        {
      		free(cmdLine);
      		continue;
    	}

    	// Prints the info struct
//        print_info( info );

    	// com contains the info. of the command before the first "|"
    	com = &info->CommArray[0];
    	if ((com == NULL) || (com->command == NULL))
    	{
      		free_info(info);
      		free(cmdLine);
      		continue;
    	}

    	// com->command tells the command name of com
    	if (isBuiltInCommand(com->command) == EXIT)
        {
      		exit(1);
    	}

        // Insert your code here / commands etc.

        // Calls help() when command is help
        if (isBuiltInCommand(com->command) == HELP)
        {
            help();
            continue;
        }

        // Executes cd() when command is cd
        if (isBuiltInCommand(com->command) == CD)
        {
            cd(com, home);
            continue;
        }

        // Executes the command pipeline
        execute_pipeline(info, com, cmdLine);

        // Waits for all other processes to terminate
        if (wait(NULL) == ERROR)
        {
            perror("wait");
            exit(errno);
        }

        free(cmdLine);
        free_info(info);
  }/* while(TRUE) */
} // main

// Gives user info about how to use the shell
int help()
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
    return SUCCESS;
}

// Implementation of cd
int cd(struct commandType *com, char *home)
{
    char curr_home[500];
    strcpy(curr_home, home);

    if (strcmp(com->VarList[1], "~") == SUCCESS)
    {
        if ((chdir(curr_home)) == ERROR)
        {
            perror("cd");
            return ERROR;
        }
    }
    else if (com->VarList[1] != NULL)
    {
        if (com->VarList[1][0] == '~')
        {
            char *path = (char *) malloc(256);
            if (com->VarList[1][1] == '/')
            {
                strcpy(path, home);
                strcat(path, &com->VarList[1][1]);
            }
            else
            {
                strcpy(path, "/home/myid/");
                strcat(path, &com->VarList[1][1]);
            }
            if ((chdir(path)) == ERROR)
            {
                perror("cd");
                return ERROR;
            }
        }
        else
        {
            if ((chdir(com->VarList[1])) == ERROR)
            {
                perror("cd");
                return ERROR;
            }
        }
    }
    return SUCCESS;
}

// Executes the pipeline
int execute_pipeline(parseInfo *info, struct commandType *com, char *cmdLine)
{
    int num_pipes = info->pipeNum;
    int num_cmds = num_pipes + 1;
    int pipe_fd[num_pipes][2];
    int pipe_num, cmd_num, input_fd, output_fd, append_fd;
    char *cmd_pipeline[num_cmds];
    char ***pipeline_args = malloc(sizeof(char**) * (num_cmds));
    pid_t pid;

    // Sets up the cmd_pipeline and pipeline_arg arrays
    for (cmd_num = 0; cmd_num < num_cmds; cmd_num++)
    {
        com = &info->CommArray[cmd_num];
        if ((com == NULL) || (com->command == NULL))
        {
            free_info(info);
            free(cmdLine);
            continue;
        }

        int num_args = com->VarNum;
        char **args = malloc(sizeof(char*) * (num_args + 1));
        cmd_pipeline[cmd_num] = com->command;

        // Builds args[]
        for(int arg_num = 0; arg_num < num_args; arg_num++)
        {
            args[arg_num] = com->VarList[arg_num];
        }
        args[num_args] = NULL;
        pipeline_args[cmd_num] = args;
    }

    UNUSED(pipe_fd);

    // Initializes the pipes
    for (pipe_num = 0; pipe_num < num_pipes; pipe_num++)
    {
        if ((pipe(pipe_fd[pipe_num])) == ERROR)
        {
            perror("pipe");
            exit(errno);
        }
    }

    // Creates child process to run command pipeline
    for (cmd_num = 0; cmd_num < num_cmds; cmd_num++)
    {
        if ((pid = fork()) == ERROR)
        {
            perror("fork");
            exit(errno);
        }
        else if (pid == 0)
        {
            // Input redirection
            if (cmd_num == 0 && info->boolInfile)
            {
                if ((input_fd = open(info->inFile, O_RDONLY)) == ERROR)
                {
                    perror("open");
                    exit(errno);
                }
                if ((dup2(input_fd, STDIN_FILENO)) == ERROR)
                {
                    perror("dup2");
                    exit(errno);
                }
            }

            // Output redirection
            if (cmd_num == num_cmds - 1 && info->boolOutfile == 1)
            {
                if ((output_fd = open(info->outFile, O_CREAT | O_WRONLY | O_TRUNC, 0644))
                	== ERROR)
                {
                    perror("open");
                    exit(errno);
                }
                if ((dup2(output_fd, STDOUT_FILENO)) == ERROR)
                {
                    perror("dup2");
                    exit(errno);
                }
            }
            else if (cmd_num == num_cmds - 1 && info->boolOutfile == 2)
            {
                if ((append_fd = open(info->outFile, O_WRONLY | O_APPEND)) == ERROR)
                {
                    perror("open");
                    exit(errno);
                }
                if ((dup2(append_fd, STDOUT_FILENO)) == ERROR)
                {
                    perror("dup2");
                    exit(errno);
                }
            }

            // Redirects STDIN
            if (cmd_num != 0)
            {
                if (dup2(pipe_fd[cmd_num - 1][READ_END], STDIN_FILENO) == ERROR)
                {
                    perror("dup2");
                    exit(errno);
                }
            }

            // Redirects STDOUT
            if (cmd_num != num_pipes)
            {
                if (dup2(pipe_fd[cmd_num][WRITE_END], STDOUT_FILENO) == ERROR)
                {
                    perror("dup2");
                    exit(errno);
                }
            }

            // Closes unneeded pipe ends
            for (pipe_num = 0; pipe_num < num_pipes; pipe_num++)
            {
                close(pipe_fd[pipe_num][READ_END]);
                close(pipe_fd[pipe_num][WRITE_END]);
            }

            execvp(cmd_pipeline[cmd_num], pipeline_args[cmd_num]);
            perror("execvp");
            exit(errno);
        }
    }

    // Close unneeded pipe ends
    for (pipe_num = 0; pipe_num < num_pipes; pipe_num++)
    {
        close(pipe_fd[pipe_num][READ_END]);
        close(pipe_fd[pipe_num][WRITE_END]);
    }

    // Waits for child processes to terminate
    for (int child = 0; child < num_pipes; child++)
    {
        if (wait(NULL) == ERROR)
        {
            perror("wait");
            exit(errno);
        }
    }
    free(pipeline_args);
    return SUCCESS;
}
