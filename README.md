# Shell Program
## Project Description
Implementation of a unix shell which supports basic shell functionalities such as reading user input, parsing commands, and executing commands.

### Features:
Command Prompt:
- displays a command prompt with the current working directory

Built-in Commands:
- cd: changes the current working directory
- exit: terminates the shell
- help: displays help information about the shell
- pwd: prints current working directory

I/O Redirection:
- can support both input and output redirection in the same command or command pipeline
- supports appending output redirection

Command Pipeline Execution
- can execute pipelines with at least 7 pipes
- can also include I/O redirection in pipeline

Auto-Completion
- can use TAB to auto-complete the rest of the command

File Tree
- organizes and stores files and directories in tree-like structure

## Requirements
- gcc
- make

## Compiling the Program:
Use the provided Makefile to compile the program

## Runnig the program
Use the compiled executable, shell.exe, by running ```./shell```

## Known Limitations & Errors:
- If chdir(), dup2(), execvp(), fork(), pipe(), open(), or wait() fails, the program encounters an error and exits
- If the input is not a command, then execvp() exits with a error
- If file for input redirection doesn't exist, open() prints error message
- If target directory for cd is not a directory, the program will encounter an error
- The command line is limited to 4096 chars which includes the prompt
