#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
	fputs("jispy> ", stdout);
	fgets(buffer, 2048, stdin);
	char * cpy = malloc(strlen(buffer)+1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}

void add_history(char* unused) {}

#else

#include <editline/readline.h>
#include <editline/history.h>

#endif

/* enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM }; */

enum {LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

typedef struct {
	int type;
	long num;
	char* err;
	char* sym;
	int count;
	stuct lval** cell;

}	lval;

lval* lval_num(long x) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_NUM;
	v->num = x;
	return v;
}

lval* lval_err(int x) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_ERR;
	v->err = malloc(strlen(m)+1);
	strcpy(v->err, m);	
	return v;
}
lval* lva_sexpr(void) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_SEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}
void lval_del(lval* v) {
	switch(v->type) {
		case LVAL_NUM: break;
		case LVAL_ERR: free(v->err); break;
		case LVAL_SYM: free(v->sym); break;
		case LVAL_SEXPR:
			for(int i = 0; i < v->count; i++){
				lval_del(v->cell[i]);
			}
			free(v->cell);
		break;
	}
	free(v);
}
lval* lval_pop(lval* v, int i) {
	lval* x = v->cell[i];
	memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count-i-1));
	v->count--;
	v->cell =realloc(v->cell, sizeof(lval*) * v->count);
	return x;	
}
lval* lval_take(lval* v, int i) {
	lval* x = lval_pop(v, i);
	lval_del(v);
	return x;
}
void lval_print(lval* v);

void lval_expr_print(lval* v, char open, char close) {
	putchar(open);
	for (int i = 0; i < v->count; i++) {
	lval_print(v->cell[i]);
	if( i != (v->count-1)) {
		putchar('  ');
	}
	}
	putchar(close);
}

void lval_print




void lval_println(lval v) { lval_print(v); putchar('\n'); }

lval eval_op(lval x, char* op, lval y ) {

	if(x.type == LVAL_ERR) { return x; }
	if(y.type == LVAL_ERR) { return y; }
	if(strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
	if(strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
	if(strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
	if(strcmp(op, "/") == 0) { 
		return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num/y.num);
	}
	return lval_err(LERR_BAD_OP);
}
		


lval eval(mpc_ast_t* t) {

	if(strstr(t->tag, "number")) {
		errno = 0;
		long x = strtol(t->contents, NULL, 10);
		return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
}
	char* op = t->children[1]->contents;
	lval x = eval(t->children[2]);
	int i = 3;
	while (strstr(t->children[i]->tag, "expr")) {
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}
	return x;
}

int main(int argc, char** argv) {
  
  	mpc_parser_t* Number = mpc_new("number");
  	mpc_parser_t* Symbol = mpc_new("symbol");
	mpc_parser_t* Sexpr = mpc_new("sexpr");
  	mpc_parser_t* Expr = mpc_new("expr");
  	mpc_parser_t* Jispy = mpc_new("jispy");
  
  	mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      symbol   : '+' | '-' | '*' | '/' ;                  \
      sexpr    : '(' <expr>* ')' ;			  \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      jispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
   	 Number, Operator, Sexpr, Expr, Jispy);
  
  	puts("Jispy Version 0.0.0.0.4");
  	puts("Press Ctrl+c to Exit\n");
  
  	while (1) {
  
    		char* input = readline("jispy> ");
    		add_history(input);
    
    		mpc_result_t r;
    		if (mpc_parse("<stdin>", input, Jispy, &r)) {
      
      			lval result = eval(r.output);
      			lval_println(result);
      			mpc_ast_delete(r.output);
      
    		} else {    
      			mpc_err_print(r.error);
      			mpc_err_delete(r.error);
    		}
    
		free(input);
    
  	}
  
  	mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Jispy);
  
  	return 0;
}
