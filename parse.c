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


#include <fcntl.h>

struct cmd_info;

void parse(char* line) {
	struct cmd_info info;
	// must initialize struct.
	struct_constructor(&info);

	// token_count determines how many tokens are in the cmd line.
	// token: is a word separted by a space.
	int token_count;
	token_count = count_tokens(line);
	// printf("%d\n", token_count);

	// allocate array of tokens
	char** tokens = (char**) malloc(sizeof(char *) * token_count);
	if (tokens != NULL) {
		tokenize_line(tokens, line, &info);
		//print_tokens(tokens, token_count);
		print_output(tokens, token_count, &info);
		// free tokens
	}

	printf("| count: %d\n", info.pipe_count);
	printf("> count: %d\n", info.input_redirection_count);
	printf("< count: %d\n", info.output_redirection_count);
	printf(">> count: %d\n", info.append_count);

}

void struct_constructor(struct cmd_info* info) {
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
		} else {
			printf("%s ", *tokens);
		}
		++tokens;
	}
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
		// redirect childs stdout > dev/null
		// i dont want execvp("which, *word") to write to stdout.
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

		if (dup2(new, 1) == -1) {
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

/*

? cat<read
1: <'read' 'cat' 

? cat <read
1: <'read' 'cat' 
*/