# Custom Shell Implementation

This project implements a basic custom shell program written in C. It allows the user to run commands, including handling pipes, redirections (`<`, `>`, `>>`), and multiple tokens. The shell is capable of forking processes and managing standard input/output redirection.

## Features

1. **Shell Prompt**: Custom shell prompt `my_shell$` is displayed, and user input is captured.
2.  **Exit Command**: The shell can exit gracefully when the user types `exit`.
3.  **Command Execution**: Commands entered by the user are executed using `execvp()`.
4.  **Input Redirection**: Supports input redirection using `<`.
5.  **Output Redirection**: Supports output redirection with `>` for overwriting the content of a file.
6.  **Append Redirection**: Supports appending output using `>>`.
7.  **Pipes**: Allows the use of pipes (`|`) to connect the output of one command to the input of another.
8.  **Memory Management**: Memory is allocated dynamically and freed appropriately to prevent memory leaks.

## Files

- **my_shell.h**: Header file that defines the function prototypes for the shell.
- **my_shell.c**: Contains the function implementations for initializing the shell, handling input, redirection, pipe execution, and memory management.
- **main.c**: Contains the main function which drives the shell, takes user input, parses commands, and manages process forking and execution.

## Requirements:

- C compiler (e.g GCC)
- Basic system libraries (stdio.h, stdlib.h, string.h, unistd.h, sys/wait.h, fcntl.h)

## Building and Running
To compile the program:
- gcc -o my_shell main.c my_shell.c

To run the program:
- ./my_shell

## Functionality

### 1. `init_shell()`
- Displays the shell prompt and waits for user input.
- Exits the shell if the user types `exit`.

### 2. `space_deletion()`
- Splits the input line into tokens based on spaces and newline characters.

### 3. Redirection Functions
- **`input_redirection_check()`**: Handles input redirection (`<`).
- **`overwrite_redirection_check()`**: Handles output redirection (`>`).
- **`append_redirection_check()`**: Handles append redirection (`>>`).

### 4. `pipe_check()`
- Detects if a pipe (`|`) exists in the command line and returns its position.

### 5. `pipe_execute()`
- Handles command execution with pipes. It separates commands before and after the pipe, forks processes, and sets up a communication channel using pipes.

### 6. `line_separation()`
- Separates the tokens of the input line into two parts, typically for handling commands before and after the pipe.

### 7. `delete_element()`
- Removes a specific token from the tokenized line array and shifts the remaining elements accordingly.

### 8. `free_lines()`
- Frees memory allocated for the line and token arrays.
