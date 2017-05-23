#ifndef PARSE_H_   /* Include guard */
#define PARSE_H_

// cmd_info struct contains info about cmd line.
// if there is any input/output redirection (the < and > characters).
// how many pipes there are on the command line (which is one less 
// than the number of commands between pipes, “who | wc” is two commands 
// separated by one pipe).
// the command line arguments are to each command

struct cmd_info {
	char** tokens;
	int token_count;
	int command_count;
	int pipe_count;
	int input_redirection_count;
	int output_redirection_count;
	int append_count;
};

void parse(char* line);
void struct_constructor(struct cmd_info* info);
int count_tokens(const char* line);
void tokenize_line(char** tokens, char* line, struct cmd_info* info);
void print_tokens(char** tokens,int token_count);
void print_output(char** tokens,int token_count, struct cmd_info* info);
void free_tokens(char** tokens, int token_count);
int match(const char *string, char *pattern);
int validate_word(char * token);
int check_command(char * word);
int check_pipe(char* token);
int check_input_redirection(char* word);
int check_output_redirection(char* word);
int check_append(char* word);
void modify_redirections(char** tokens, int token_count);
int get_end_of_command(int start_of_command, int* next_starting_command, int token_count, char** tokens);
void modify_input_redirection(int start_of_command, int end_of_command, char** tokens);
void shift_right(char** tokens, int start, int end);
void modify_output_redirection(int start_of_command, int end_of_command, char** tokens);
void shift_left(char** tokens, int start, int end);

#endif // PARSE_H_