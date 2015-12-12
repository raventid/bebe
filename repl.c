#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048]

/*Fake readline function*/
char* readline(char* prompt) {
 fputs(prompt, stdout);
 fgets(buffer, 2048, stdin);
 char* cpy = malloc(strlen(buffer)+1);
 strcpy(cpy, buffer);
 cpy[strlen(cpy)-1]='\0';
 return cpy;
}

/*Fake add_history function*/
void add_history(char* unsued) {}

/*if it is not windows include the editline headers*/
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int number_of_nodes(mpc_ast_t* t) {
 if (0 == t->children_num) { return 1;}
 /*@todo consider posibility of using else here instead of new if*/
 if (1 <= t->children_num) {
  int total = 1;
  for (int i = 0; i < t->children_num; i++) {
   total = total + number_of_nodes(t->children[i]); 
  }
  return total;
 }
 /* This is a stub for compiler and this return should never be reached*/
 return 0;
}

/*Use operator string to see which operation to perform*/
long eval_op(long x, char* op, long y) {
 if (0 == strcmp(op, "+")) { return x + y; }
 if (0 == strcmp(op, "-")) { return x - y; }
 if (0 == strcmp(op, "*")) { return x * y; }
 if (0 == strcmp(op, "/")) { return x / y; }
 return 0;
}

long eval(mpc_ast_t* t) {
 /* If tagged as number return it directly. */
 if (strstr(t->tag, "number")) {
  return atoi(t->contents); 
 }

 /* The operator is always second child.  */
 char* op = t->children[1]->contents;

 /* We store the third child in `x`  */
 long x = eval(t->children[2]);

 /* Iterate the remaining children and combining  */
 int i = 3;
 while(strstr(t->children[i]->tag, "exp")) {
  x = eval_op(x, op, eval(t->children[i]));
  i++;
 }

 return x;
}

int main(int argc, char** argv) {

mpc_parser_t* Number = mpc_new("number");
mpc_parser_t* Operator = mpc_new("operator");
mpc_parser_t* Expr = mpc_new("expr");
mpc_parser_t* Bebe = mpc_new("bebe");

mpca_lang(MPCA_LANG_DEFAULT,
	"                                               \
	number   : /-?[0-9]+/ ;                         \
        operator : '+' | '-' | '*' | '/' ;              \
        expr     : <number> | '('<operator><expr>+')' ; \
        bebe     : /^/ <operator> <expr>+ /$/ ;         \
        ",	
	Number,Operator,Expr,Bebe);

puts("bebe Version 0.0.3");
puts("Press Ctrl+c to Exit\n");

while (1) {
 char *input = readline("bebe> ");

 add_history(input); 

 mpc_result_t result;
 if(mpc_parse("<stdin>", input, Bebe, &result)){
  /*Success, let's print the correct AST*/
  long res = eval(result.output);
  printf("%li\n", res);
  //we might output ast tree structure while debbuging, perhaps make a constant here
  //mpc_ast_print(result.output); 
  mpc_ast_delete(result.output);
 } else {
  /*Failure, let's print what went wrong*/
  mpc_err_print(result.error);
  mpc_err_delete(result.error); 
 }


 free(input);
}

/*We should not forget to return memory back to OS :)*/
mpc_cleanup(4, Number, Operator, Expr, Bebe);
return 0;
}
