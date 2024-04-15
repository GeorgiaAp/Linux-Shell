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
    char* line;
    char** new_line;
    
    while(1)
    {   
        oldin = dup(STDIN_FILENO);
        oldout = dup(STDOUT_FILENO);
        
        line = init_shell();
        new_line = space_deletion(line);

        pid=fork();
        if(pid==-1)
        {   
            perror("fork");
            return(1);
        }
        if(pid==0)
        {   
            ret_val = pipe_check(new_line);
            if(ret_val!=0)
                new_line = pipe_execute(new_line, ret_val);
            
            new_line = input_redirection_check(new_line);
            new_line = overwrite_redirection_check(new_line);
            new_line = append_redirection_check(new_line);

            execvp(new_line[0],new_line);
            perror("execvp");
            return(1);
        }
        waitpid(pid,&status,0);
        if (WIFEXITED(status))
            printf("child returned %d\n", WEXITSTATUS(status));
        else 
            printf("child terminated abnormally\n");
        
        dup2(oldin, STDIN_FILENO);
        dup2(oldout, STDOUT_FILENO);
        close(oldin);
        close(oldout);
        close(fd);

        fflush(stdin);
        fflush(stdout);
    }
    
    free_lines(line, new_line);
    return 0;
}