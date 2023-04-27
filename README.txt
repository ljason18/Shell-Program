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

Usage:
    Run a unix shell implemenatation that supports basic shell funcionalities including:
    - reading user input
    - parsing commands
    - executing commands

Features:
    Command Prompt:
    - displays a command prompt with the current working directory

    Built-in Commands:
    - cd: changes the current working directory
    - exit: terminates the shell
    - help: displays help information about the shell
    - pwd: prints current working directory

    I/O Redirection:
    - can support both input and output redirection in the same command or command
      pipeline
    - supports appending output redirection

    Command Pipeline Execution
    - can execute pipelines with at least 7 pipes
    - can also include I/O redirection in pipeline

    Auto-Completion
    - can use TAB to auto-complete the rest of the command

    File Tree
    - organizes and stores files and directories in tree-like structure

Implementation Details:
    - command execution implmented using fork(), exec(), and wait()
    - use pipe() to execute command pipelines
    - use dup2() and close() for redirecting STDIN and STDOUT and with I/O redirection
    - prints errors to stderr using perror() followed by exit();
    - implemented cd using chdir();
    - used the provided parser to parse the command line
    - edited the parser to support reading anything within '' as a single argument

Known Limitations & Errors:
    If chdir(), dup2(), execvp(), fork(), pipe(), open(), or wait() fails, the program
    encounters an error

    If the input is not a command, then execvp() exits with a error

    If file for input redirection doesn't exist, open() prints error message

    If target directory for cd is not a directory, it is an error

    The command line is limited to 4096 chars which includes the prompt
