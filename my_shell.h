#ifndef MY_SHELL_H
#define MY_SHELL_H

// Function to initialize the shell prompt and take user input
// It will aldo handle the "exit" command
char* init_shell();

// Function to split the input line into tokens based on the spaces nd newlines
char** space_deletion(char *line);

// Function to check if there is input redirection ("<") in the command
// If so, it modifies the input line and redirects input from the specific file
char** input_redirection_check(char** line);

// Function to check if there is output redirection (">") in the command
// If so, it modifies the input line and redirects output from the specified file (overwriting it)
char** overwrite_redirection_check(char** line);

// Function to check if there is append redirection (">>") in the command
// If so, it modifies the input line and redirects output to the specified file (appending to it) 
char** append_redirection_check(char** line);

// Function to delete an element (token) at a specified position in the line array
// It shifts the remaining tokens to the left to fill the gap
char** delete_element(char** line, int pos);

// Function to check if ther is a pipe ("|") operator in the command
// it returns the position of the pipe operator in the token list
int pipe_check(char** line);

// Funtion to handle the execution of commands with a pipe ("|") operator
// It separates the inpt line into two parts (before and after the pipe),
// sets up a pipe for communication between the two parts, and executes the command in child processes
char** pipe_execute(char** line, int pos);

// Function to separate the input line into two parts based on the start and end indices
// Used in handling pipes to separate commands before and after the pipe operator
char** line_separation(char** line, int start, int end);

//Function to free the memory allocated for the input file and its tokenized version
void free_lines(char* line, char** new_line);

#endif
