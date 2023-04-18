#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "go_shell.h"
#include "parse.h"

int main()
{
    goshell();
    return 0;
}

int goshell()
{
    parseInfo *p;
    char *buffer = (char *) malloc(256);
    int pid;
    do
    {
        printf("gosh@HW05> ");
        scanf("%s", buffer);
        p = parse(buffer);
//        print_info(p);
//        printf("%s\n", p->CommArray[0].command);
        if ((pid = fork()) < 0)
        {
            perror("fork");
            exit(errno);
        }
        else if (pid == 0)
        {
            execvp(p->CommArray[0].command, p->CommArray[0].VarList);
        }
        else if (pid > 0)
        {
            wait(NULL);
        }
    }
    while (strcmp(p->CommArray[0].command, "exit") != 0);
    free_info(p);
    return 0;
}
