#ifndef PARSE_H_   /* Include guard */
#define PARSE_H_

// cmd_info struct contains info about cmd line.
// if there is any input/output redirection (the < and > characters).
// how many pipes there are on the command line (which is one less 
// than the number of commands between pipes, “who | wc” is two commands 
// separated by one pipe).
// the command line arguments are to each command

struct cmd_info {
	int command_count;
};

void parse(char* line);
int count_tokens(const char* line);
void tokenize_line(char** tokens, char* line);
void print_tokens(char** tokens,int token_count);
void print_output(char** tokens,int token_count);
int match(const char *string, char *pattern);
int validate_word(char * token);
int check_command(char * word);

#endif // PARSE_H_