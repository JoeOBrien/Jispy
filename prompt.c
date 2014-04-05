#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32

#include <string.h>

static char buffer[2048];
char* readline(char* prompt) {
	fputs("jispy> ", stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer)+1)
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}

void add_history(char* unused) {}

#else

#include <editline/readline.h>
#include <editline/history.h>

#endif

int main(int argc, char** argv) {
	puts("Jispy Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");
	while(1){
		char* input = readline("jispy> ");
		add_history(input);
		printf("No you're a %s\n", input);
		free(input);			
	}
	return 0;
}
