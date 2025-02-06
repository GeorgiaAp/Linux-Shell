#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "my_shell.h"
#include "my_shell.c"


int main(int argc, char* argv[])
{   
    int ret_val, pd[2], pid, status, fd, oldin, oldout;
    char* line;    // Variable to store the user input file
    char** new_line;    // Array of tokens parsed from the input line
    
    while(1)    // Main loop runs the shell until the user exits
    {   
        // Save the current standard input and output file descriptors
        oldin = dup(STDIN_FILENO);
        oldout = dup(STDOUT_FILENO);

        // Display the shell prompt and get user input
        line = init_shell();
        // Tokenize the input line (split by spaces, tabs and newlines)
        new_line = space_deletion(line);

        // Fork a new process to execute the command
        pid=fork();
        if(pid==-1)
        {   
            perror("fork");
            return(1);
        }
        
        // In the child process:
        if(pid==0)
        {   
            // Check if the command includes a pipe operator "|" 
            ret_val = pipe_check(new_line);
            if(ret_val!=0)    // If a pipe is found, handle the pipe execution
                new_line = pipe_execute(new_line, ret_val);

            // Check and handle input/output redirection ("<", ">", ">>")
            new_line = input_redirection_check(new_line);
            new_line = overwrite_redirection_check(new_line);
            new_line = append_redirection_check(new_line);

            // Execute the command
            execvp(new_line[0],new_line);
            perror("execvp");
            return(1);
        }
        
        // In the parent process:
        waitpid(pid,&status,0);    // Wait for the child to finish
        if (WIFEXITED(status))
            printf("child returned %d\n", WEXITSTATUS(status));
        else 
            printf("child terminated abnormally\n");
        
        // Restore the original standard input and output file descriptors
        dup2(oldin, STDIN_FILENO);
        dup2(oldout, STDOUT_FILENO);
        // Close the saved file descriptors
        close(oldin);
        close(oldout);
        close(fd);
        // Flush the input and output buffers
        fflush(stdin);
        fflush(stdout);
    }
    // Free allocated memory
    free_lines(line, new_line);
    
    return 0;
}
