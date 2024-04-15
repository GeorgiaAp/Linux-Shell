#ifndef MY_SHELL_H
#define MY_SHELL_H

char* init_shell();
char** space_deletion(char *line);
char** input_redirection_check(char** line);
char** overwrite_redirection_check(char** line);
char** append_redirection_check(char** line);
char** delete_element(char** line, int pos);
int pipe_check(char** line);
char** pipe_execute(char** line, int pos);
char** line_separation(char** line, int start, int end);
void free_lines(char* line, char** new_line);

#endif