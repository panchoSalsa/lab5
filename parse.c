// source=http://stackoverflow.com/questions/7109964/creating-your-own-header-file-in-c
#include "parse.h"

//  please include the header <stdio.h> or explicitly provide a declaration for 'printf'
//1 warning generated.
#include <stdio.h>

#include <string.h>
// i need #include <string.h> to avoid the following error caused by 
// strtok()

// strtok.c: In function ‘main’:
//strtok.c:8:7: warning: assignment makes pointer from integer without a cast [enabled by default]
//   pch = strtok (str," ,.-");

#include <stdlib.h>
// parse.c:28:18: note: please include the header <stdlib.h> or explicitly provide a declaration for
// 'malloc'

#include <regex.h>


#include <fcntl.h> // for open
#include <unistd.h> // for close

struct cmd_info;

void parse(char* line) {
	struct cmd_info info;
	// must initialize struct.
	struct_constructor(&info);

	// token_count determines how many tokens are in the cmd line.
	// token: is a word separted by a space.
	// int token_count;
	info.token_count = count_tokens(line);
	// printf("%d\n", token_count);

	// allocate array of tokens
	// char** tokens = (char**) malloc(sizeof(char *) * token_count);
	info.tokens = (char**) malloc(sizeof(char *) * info.token_count);

	if (info.tokens != NULL) {
		tokenize_line(info.tokens, line, &info);

		// this function prints each token in a separate line.
		// useful for debugging.
		//print_tokens(info.tokens, info.token_count);

		modify_redirections(info.tokens, info.token_count);

		print_output(info.tokens, info.token_count, &info);

		// free tokens

		// TO-DO

		//
		

	} else {
		perror("malloc() error");
	}
}

void struct_constructor(struct cmd_info* info) {
	info->tokens = NULL;
	info->token_count = 0;
	info->command_count = 0;
	info->pipe_count = 0;
	info->input_redirection_count = 0;
	info->output_redirection_count = 0;
	info->append_count = 0;
}


// returns the number of tokens in line argument.
// tokens are separted by blank spaces.
// source=http://stackoverflow.com/questions/12698836/counting-words-in-a-string-c-programming
int count_tokens(const char* line) {
	int counted = 0; // result

	// state:
	const char* it = line;
	int inword = 0;

	do switch(*it) {
		case '\0': 
		case ' ': case '\t': case '\n': case '\r': // TODO others?
			if (inword) { inword = 0; counted++; }
			break;
		default: inword = 1;
	} while(*it++);

	return counted;
}


// source=http://www.tutorialspoint.com/ansi_c/c_strtok.htm
void tokenize_line(char** tokens, char* line, struct cmd_info* info) {
	char * pch;
	pch = strtok (line," ");
	// source=http://stackoverflow.com/questions/19342155/how-to-store-characters-into-a-char-pointer-using-the-strcpy-function
	// source=https://www.tutorialspoint.com/c_standard_library/c_function_strlen.htm
	while (pch != NULL) {
		// printf ("%s",pch);
		// allocate space for the token returned by strtok().
		// strlen(pch) + 1) because strlen omits the '\0' but we need it in order
		// to deal with strings.
		// *tokens is a char*
		// source=http://stackoverflow.com/questions/7812805/how-to-malloc-char-table
		*tokens = (char*) malloc ( (strlen(pch) + 1) * sizeof(char));
		strcpy(*tokens,pch);

		if (check_command(*tokens)) {
			++(info->command_count);
		}

		if (check_pipe(*tokens) == 0) {
			++(info->pipe_count);
		}

		if (check_input_redirection(*tokens) == 0) {
			++(info->input_redirection_count);
		}

		if (check_output_redirection(*tokens) == 0) {
			++(info->output_redirection_count);
		}

		if (check_append(*tokens) == 0) {
			++(info->append_count);
		}

		// advance the pointer to next char* 
		++tokens;

		pch = strtok (NULL, " ");

		// if (pch != NULL) {
		// 	printf("\n");
		// }
	}
	// printf("\n");
}

void print_tokens(char** tokens,int token_count) {
	printf("printing tokens\n");
	int i;
	for (i = 0; i < token_count; ++i) {
		printf("%s ", *tokens);
		++tokens;
	}
	printf("\n");
}


void print_output(char** tokens,int token_count, struct cmd_info* info) {
	// display number of valid commands
	printf("%d: ", info->command_count);
	int i;
	for (i = 0; i < token_count; ++i) {
		if (validate_word(*tokens)) {
			printf("'%s' ", *tokens);
		} else if ( (check_input_redirection(*tokens) == 0) || (check_append(*tokens) == 0) || (check_output_redirection(*tokens) == 0) ) {
			printf("%s", *tokens);
		} else if ( (check_pipe(*tokens) == 0) ) {
			printf("%s ", *tokens);
		}
		// } else {
		// 	printf("%s ", *tokens);
		// }
		++tokens;
	}
	printf("\n");

	// printf("| count: %d\n", info->pipe_count);
	// printf("< count: %d\n", info->input_redirection_count);
	// printf("> count: %d\n", info->output_redirection_count);
	// printf(">> count: %d\n", info->append_count);

	printf("\n");
}

// source=http://stackoverflow.com/questions/1631450/c-regular-expression-howto
// regex helper function needed to detect if options are present [-N,M]
int match(const char *string, char *pattern) {
    int    status;
    regex_t    re;

    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
        return(0);      /* Report error. */
    }
    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);
    if (status != 0) {
        return(0);      /* Report error. */
    }
    return(1);
}

int validate_word(char * token) {

	// regex to match word "^[a-zA-Z0-9\\-]+"
	// need to escape - in regex using \\- 
	//source=http://stackoverflow.com/questions/18477153/c-compiler-warning-unknown-escape-sequence-using-regex-for-c-program
	if ( match(token,"^[a-zA-Z0-9\\-]+") ) {
		// printf("match\n");
		return 1;
	}
	else {
		// printf("no match\n");
		return 0;
	}
}

// source=http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/exec.html
// source=http://stackoverflow.com/questions/8035372/c-runtime-test-if-executable-exists-in-path
// using 'which' command to test if command is in $PATH
int check_command(char * word) {
	pid_t  pid;
	int    status;
	char* argv[3];
	// execvp("which", "which, *word, NULL");
	argv[0] = "which";
	argv[1] = word;
	argv[2] = NULL;

	if ((pid = fork()) < 0) {     /* fork a child process           */
		perror("fork() error");
		exit(1);
	}
	else if (pid == 0) {          /* for the child process:         */

		// i dont want execvp("which, *word") to write to the console
		// so i redirect stdout  and stderr > /dev/null

		// since i just care about the return code of the which command i can
		// ignore writes to stdout.
		// source=http://stackoverflow.com/questions/4832603/how-could-i-temporary-redirect-stdout-to-a-file-in-a-c-program
		// source=https://linux.die.net/man/3/open
		int new;
		new = open("/dev/null", O_WRONLY);

		if (new < 0) {
			perror("open() error");
			exit(1);
		}

		// redirect childs stdout > /dev/null
		if (dup2(new, 1) == -1) {
			perror("dup2() error");
		}

		// redirect childs stderr > /dev/null
		if (dup2(new, 2) == -1) {
			perror("dup2() error");
		}

		if (close(new) == -1) {
			perror("close() error");
		}

		// execvp("which", "which, *word, NULL");
		if (execvp(argv[0],argv) < 0) {     /* execute the command  */
			perror("execvp() error");
			exit(1);
		}
	}
	else {                                  /* for the parent:      */
		/* wait for completion  */
		while (wait(&status) != pid) ;

		// status == 0 means valid shell script or executable.
		// waiting to recieve status of child execvp on "which *word"
		// should return 0 if argument to which is a valid executable file
		// or shell script in $PATH 
		if ( pid && (status == 0) ) {
			return 1;
		}
	}
	return 0;
}

int check_pipe(char* word) {
	return strcmp(word,"|");
}

int check_input_redirection(char* word) {
	return strcmp(word,"<");
}

int check_output_redirection(char* word) {
	return strcmp(word,">");
}

int check_append(char* word) {
	return strcmp(word,">>");
}


void modify_redirections(char** tokens, int token_count) {
	int start_of_command, next_starting_command, end_of_command;
	// start_of_command = 0;
	// int next_starting_command;
	// next_starting_command = 0;
	// int end_of_command;
	start_of_command = next_starting_command =end_of_command = 0;
	while ( (end_of_command = get_end_of_command(start_of_command, &next_starting_command, token_count, tokens)) != -1 ) {
		modify_input_redirection(start_of_command, end_of_command, tokens);
		modify_output_redirection(start_of_command, end_of_command, tokens);

		// go to the next command 
		start_of_command = next_starting_command;
	}
}

// this function is called the first time with *start = 0, start at first token
// the next time it is called start points either to a token after a pipe ('|')
// or start is >= size meaning that it processed all the tokens and it needs 
// to notify the programmer using a -1.

// returns the position of a token before a pipe ('|')

// if it finishes all tokens it returns the last position of the token, size - 1
// size -1 ==> position of last token

int get_end_of_command(int start_of_command, int* next_starting_command, int token_count, char** tokens) {
	// check if you are at the end of tokens
	if (start_of_command >= token_count) {
		return -1;
	}

	int i;
	for (i = start_of_command; i < token_count; ++i) {
		if (strcmp(tokens[i],"|") == 0) {

			// change starting location for next call
			// next starting position is one token after the pipe ('|')
			*next_starting_command = i + 1;

			// end_of_command will be set to this value (i - 1).
			// (i - 1) is one token before the pipe ('|').
			return (i - 1); 
		}
	}

	// setting *next_start = token_count will terminate the while loop because
	// in the next call start >= token_count
	*next_starting_command = token_count;

	// return position of last token
	return token_count - 1;
}

void modify_input_redirection(int start_of_command, int end_of_command, char** tokens) {
	int i;
	// itertating from command to pipe
	for (i = start_of_command; i < end_of_command; ++i) {
		if ( (strcmp(tokens[i],"<") == 0) && (i != start_of_command)) {
			// printf("found <\n");

			// we are currently pointing to "<" token
			char* redirection = tokens[ i ];
			char* filename = tokens[ i + 1 ];

			// we will shift all the tokens to the right.
			// i + 1 because we want to begin shifting and replacing
			// the filename first, the "<" second ...
			shift_right(tokens, start_of_command, i + 1);

			tokens[ start_of_command ] = redirection;
			tokens[ start_of_command + 1 ] = filename; 
			return;
		}
	}
}

void shift_right(char** tokens, int start, int end) {
	int i;

	for (i = end; i > start; --i) {
		tokens[i] = tokens[i - 2];
	}
}

void modify_output_redirection(int start_of_command, int end_of_command, char** tokens) {
	int i;
	// itertating from command to pipe
	for (i = start_of_command; i < end_of_command; ++i) {
		// if token[i] == '>' and i == end - 1 then dont do anything
		// since the tokens ( > 'filename') are at the end
		if ( ( (strcmp(tokens[i],">") == 0) || (strcmp(tokens[i],">>") == 0) ) && (i != end_of_command - 1)) {
			// printf("found >\n");


			// we are currently pointing to ">" token
			char* redirection = tokens[ i ];
			char* filename = tokens[ i + 1 ];

			// we will shift all the tokens to the left.
			// i - 1 because we want to begin shifting from the end of 
			// the command
			shift_left(tokens, i, end_of_command - 1);

			tokens[ end_of_command - 1 ] = redirection;
			tokens[ end_of_command ] = filename; 
			return;
		}
	}
}


void shift_left(char** tokens, int start, int end) {
	int i;

	for (i = start; i < end; ++i) {
		tokens[i] = tokens[i + 2];
	}
}

/*

? cat<read
1: <'read' 'cat' 

? cat <read
1: <'read' 'cat' 
*/