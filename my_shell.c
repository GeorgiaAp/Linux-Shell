#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "my_shell.h"

char* init_shell()
{
    char* line;
    line = (char*)malloc(sizeof(char)*100);
    
    printf("my_shell$ ");
    fgets(line, 100, stdin);

    if(strcmp(line,"exit\n")==0)
    {   
        printf("GOODBYE!\n");
        exit(0);
    } 
    return line;            
}

char** space_deletion(char *line)
{
    int i=0;
    char** new_line;

    new_line = (char**)malloc(sizeof(char*)*1);
    for(i=0; i<10; i++)
        new_line[i] = (char*)malloc(sizeof(char)*20);
    i=0;
    
    new_line[0] = strtok(line, " \n\0");
    while(new_line[i]!=NULL)
    {
        i++;
        new_line[i] = strtok(NULL, " \n\0");
    }
    
    return new_line;
}

char** input_redirection_check(char** line)
{
    int fd, oldin;
    int i=0;

    for(i=0; line[i]!=NULL; i++)
    {
        if(strcmp(line[i], "<")==0)
        {
            fd = open(line[i+1], O_RDONLY);
            if(fd<0)
                perror("open");
                
            line = delete_element(line, i);
            dup2(fd,STDIN_FILENO);
            return line;
        }
    }
    return line;
}

char** overwrite_redirection_check(char** line)
{
    int fd, oldout;
    int i=0;

    for(i=0; line[i]!=NULL; i++)
    {
        if(strcmp(line[i], ">")==0)
        {
            fd = open(line[i+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if(fd<0)
                perror("open");

            line = delete_element(line, i);
            dup2(fd,STDOUT_FILENO);
            return line;
        }
    }
    return line;
}

char** append_redirection_check(char** line)
{
    int fd, oldout;
    int i=0;

    for(i=0; line[i]!=NULL; i++)
    {
        if(strcmp(line[i], ">>")==0)
        {
            fd = open(line[i+1], O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
            if(fd<0)
                perror("open");

            line = delete_element(line, i);
            dup2(fd,STDOUT_FILENO);
            return line;
        }
    }
    return line;
}

char** delete_element(char** line, int pos)
{
    int i=0;

    for (i = pos; line[i]!=NULL; i++)
        line[i] = line[i+2];
    
    return line;
}

int pipe_check(char** line)
{
    int fd, i, pd[2], oldin, oldout;
    char **line1, **line2;

    for(i=0; line[i]!=NULL; i++)
    {
        if(strcmp(line[i], "|")==0)
            return i;
    }
    return 0;
}

char** line_separation(char** line, int start, int end)
{
    int i=0;
    char **token;

    token = (char**)malloc(sizeof(char*)*1);

    for(i=0; start<end; i++)
    {
        token[i] = (char*)malloc(sizeof(char)*20);
        token[i] = line[start++];
        if(token[i]==NULL)
            break;   
    }
    return token;
}

char** pipe_execute(char** line, int pos)
{
    int pid, status, pd[2], oldin, oldout;
    char **line1, **line2;

    line1 = line_separation(line, 0, pos);
    line2 = line_separation(line, pos+1, sizeof(line));
    
    pipe(pd);
    if(pd<0)
        perror("pipe");
    
    pid = fork();
    if(pid<0)
        perror("fork");
    if(pid==0)
    {   
        oldout = dup(1);
        dup2(pd[1],1);
        close(pd[0]);
        
        execvp(line1[0], line1);
        perror("error in the first exec of pipe.");
    }
    waitpid(pid,&status,0);
    if (WIFEXITED(status))
        printf("child returned %d\n", WEXITSTATUS(status));
    else
        printf("child terminated abnormally\n");

    close(pd[1]);
    dup2(oldout,1);
    close(oldout);
        
    oldin = dup(STDIN_FILENO);
    dup2(pd[0], STDIN_FILENO);
    close(pd[1]);

    return line2;
}

void free_lines(char* line, char** new_line)
{
    int i=0;

    free(line);
    for(i=0; i<sizeof(new_line); i++)
    {
        free(new_line[i]);
    }
    free(new_line);
}