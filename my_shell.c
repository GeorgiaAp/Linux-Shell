#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "my_shell.h"

// Function to initialize the shell prompt, take user input and handle "exit" command
char* init_shell()
{
    char* line;
    line = (char*)malloc(sizeof(char)*100);    // Allocate memory for the input lin
    
    printf("my_shell$ ");
    fgets(line, 100, stdin);    // Take input from the user

    // If the user types "exit", the shell will terminate
    if(strcmp(line,"exit\n")==0)
    {   
        printf("GOODBYE!\n");
        exit(0);    // Exit the shell
    } 
    return line;    // Return the input line
}

// Function to split the input line by spaces and newline characters
char** space_deletion(char *line)
{
    int i=0;
    char** new_line;

    // Allocate memory for spliting the input line into tokens
    new_line = (char**)malloc(sizeof(char*)*1);
    for(i=0; i<10; i++)
        new_line[i] = (char*)malloc(sizeof(char)*20);
    i=0;

    // Tokenize the input line by spaces, newlines and null characters
    new_line[0] = strtok(line, " \n\0");
    while(new_line[i]!=NULL)
    {
        i++;
        new_line[i] = strtok(NULL, " \n\0");
    }
    
    return new_line;    // Return the array of tokens
}

// Function to check for input redirection (<)
char** input_redirection_check(char** line)
{
    int fd, oldin;
    int i=0;

    // Go through the command tokens to find input redirection "<"
    for(i=0; line[i]!=NULL; i++)
    {
        if(strcmp(line[i], "<")==0)
        {
            fd = open(line[i+1], O_RDONLY);    // Open the file for reading
            if(fd<0)
                perror("open");
                
            line = delete_element(line, i);    // Remove "<" and its argument from the token list
            dup2(fd,STDIN_FILENO);    // Redirect input to the file
            return line;    // Return the modified token list 
        }
    }
    return line;    // Return the original list if no redirection is found
}

// Function to check for  output redirection (>)
char** overwrite_redirection_check(char** line)
{
    int fd, oldout;
    int i=0;

    // Go through the command tokens to find output redirection ">"
    for(i=0; line[i]!=NULL; i++)
    {
        if(strcmp(line[i], ">")==0)
        {
            // Open the file for writing, create if not exists, overwrite existing content
            fd = open(line[i+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if(fd<0)
                perror("open");

            line = delete_element(line, i);    // Remove ">" and its argument from the token list
            dup2(fd,STDOUT_FILENO);    // Redirect output to the file
            return line;    // Return the modified token list
        }
    }
    return line;    // Return the original list if no redirection is found
}

// Function to check for append output redirection (>>)
char** append_redirection_check(char** line)
{
    int fd, oldout;
    int i=0;

    // Go through the command tokens to find append redirection ">>"
    for(i=0; line[i]!=NULL; i++)
    {
        if(strcmp(line[i], ">>")==0)
        {
            // Open the file for appending, create if not exists
            fd = open(line[i+1], O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
            if(fd<0)
                perror("open");

            line = delete_element(line, i);    // Remove ">>" and its arguments from the token list
            dup2(fd,STDOUT_FILENO);    // Redirect output to the file
            return line;    // Return the modified token list
        }
    }
    return line;    // Return the original list if no redirection is found
}

// Function to delete an element at a specific position in the token list 
char** delete_element(char** line, int pos)
{
    int i=0;

    // Shift all the elements after the position to the left, deleting the element
    for (i = pos; line[i]!=NULL; i++)
        line[i] = line[i+2];
    
    return line;    // Return the modified token list
}

// Function to check if the input has a pipe ("|") operator
int pipe_check(char** line)
{
    int fd, i, pd[2], oldin, oldout;
    char **line1, **line2;
    
    // Go through the command tokens to find the pipe "|"
    for(i=0; line[i]!=NULL; i++)
    {
        if(strcmp(line[i], "|")==0)
            return i;    // Return the position of the pipe operator
    }
    return 0;    // Return 0 if no pipe is found
}

// Function to separate the command tokens into two parts for the pipe execution
char** line_separation(char** line, int start, int end)
{
    int i=0;
    char **token;

    token = (char**)malloc(sizeof(char*)*1);

    // Separate the tokens between the given start and end positions 
    for(i=0; start<end; i++)
    {
        token[i] = (char*)malloc(sizeof(char)*20);
        token[i] = line[start++];
        if(token[i]==NULL)
            break;   
    }
    return token;    //Return the separated tokens
}

// Function to handle the pipe execution between two commands
char** pipe_execute(char** line, int pos)
{
    int pid, status, pd[2], oldin, oldout;
    char **line1, **line2;

    // Separate the tokens into two parts, before and after the pipe
    line1 = line_separation(line, 0, pos);
    line2 = line_separation(line, pos+1, sizeof(line));

    // Create a pipe
    pipe(pd);
    if(pd<0)
        perror("pipe");
    
    pid = fork();    // Create a child process
    if(pid<0)
        perror("fork");
    if(pid==0)    // In the child process
    {   
        oldout = dup(1);    // Save the current stdout
        dup2(pd[1],1);    // Redirect stdout to the pipe's write end
        close(pd[0]);
        
        execvp(line1[0], line1);    // Execute the first command
        perror("error in the first exec of pipe.");
    }
    waitpid(pid,&status,0);    // Wait for the child process to finish
    if (WIFEXITED(status))
        printf("child returned %d\n", WEXITSTATUS(status));
    else
        printf("child terminated abnormally\n");

    close(pd[1]);    // Close the write end of the pipe
    dup2(oldout,1);    // Restore the original stdout
    close(oldout);
        
    oldin = dup(STDIN_FILENO);    // Save the current stdin
    dup2(pd[0], STDIN_FILENO);    // Redirect stdin to the pipe's read end
    close(pd[1]);

    return line2;    //Return the tokens after the pipe
}

// Function to free the memory allocated for the line and the token list
void free_lines(char* line, char** new_line)
{
    int i=0;

    free(line);    // Free the memory allocated for the input line
    for(i=0; i<sizeof(new_line); i++)    // Free memory for each token in the token list
    {
        free(new_line[i]);
    }
    free(new_line);    // Free the token list itself
}
