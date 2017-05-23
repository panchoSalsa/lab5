// compile
// gcc main.c parse.c

#include <stdio.h>
#include "parse.h" /* Include the header here, to obtain the function declaration */
#include <string.h>
#define BUFFER_SIZE 256

void nsh();
void remove_newline(char* line);
char* read_line(char* line, int buffer_size);

void nsh() {
	// line buffer for stdin input
	char line[BUFFER_SIZE];
	do {
		// print the nsh prompt
		printf("? ");
	// } while(fgets(line, BUFFER_SIZE, stdin));
	} while(read_line(line, BUFFER_SIZE));

	printf("\n");
}

// source=http://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
void remove_newline(char* line) {
	size_t ln = strlen(line) - 1;
	if (*line && line[ln] == '\n'){
		line[ln] = '\0';
	}
}

char* read_line(char* line, int buffer_size) {
	// fgets returns NULL when it encounters EOF
	// source=http://stackoverflow.com/questions/19228645/fgets-and-dealing-with-ctrld-input
	if (fgets(line, BUFFER_SIZE, stdin)) {
		// removing '\n' from line because when printing the last token
		// in the line i dont want printf() to display the '\n'
		remove_newline(line);
		parse(line);
		// returning a non NULL pointer in this case line so that 
		// the nsh while loop continues
		return line;
	} else {
		// EOF was encountered by fgets.
		// return NULL to force the while loop in nsh to terminate
		return NULL;
	}
}

int main(int argc, char *argv[]) {
	// run nsh shell
	nsh();
	return 0; 
}