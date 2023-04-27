Source files include:
    Makefile
    parse.c
    parse.h
    shell.c

How to compile the program:
    Can be compiled with the provided Makefile by using the make command OR
    Manually compile using gcc command with -Wall and -pedantic options

Assumptions:
    Use ./shell to run the program

    The program comes with 3 built in commands
    - cd: changes the current working directory
    - exit: terminates the shell
    - help: displays help information about the shell

    Supports I/O redirection along with appending output redirection

    Supports executing command pipelines
    - can execute pipelines with at least 7 pipes
    - can also include I/O redirection in pipeline


Known Limitations & Errors:
    If chdir(), dup2(), execvp(), fork(), pipe(), open(), or wait() fails, the program
    encounters an error

    If the input is not a command, then execvp() exits with a error

    If file for input redirection doesn't exist, open() prints error message

    If target directory for cd is not a directory, it is an error

    The command line is limited to 4096 chars which includes the prompt
