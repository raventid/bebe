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

/*Possible lval types*/
enum { LVAL_NUM, LVAL_ERR };

/*Possible error types*/
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/**/
typedef struct {
 int type;
 long num;
 int err;
} lval;

lval lval_num(long x) {
 lval v;
 v.type = LVAL_NUM;
 v.num = x;
 return v;
}

lval lval_err(int x) {
 lval v;
 v.type = LVAL_ERR;
 v.err = x;
 return v;
}

void lval_print(lval v) {
 switch(v.type) {
  /* in the case the type is a number print it */
  /* then 'break' out of the switch. */
  case LVAL_NUM: printf("%li", v.num); break;

  /* in the case the type is an error */
  case LVAL_ERR:
   /* check what type of error it is and print it  */
   if(v.err == LERR_DIV_ZERO) {
    printf("Error: Division by zero"); 
   }
   if(v.err == LERR_BAD_OP) {
    printf("Error: Invalid operator"); 
   }
   if(v.err == LERR_BAD_NUM) {
    printf("Error: Invalid number"); 
   }
  break;
 }
}

void lval_println(lval v) {
 lval_print(v);
 putchar('\n');
}
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

// integer exponentiation; 
// may not be more efficient than repeated multiplication, for small n 
// (a small lookup table would be much quicker!) 
// refer: http://www-mitpress.mit.edu/sicp/chapter1/node15.html 
// and http://www.uvsc.edu/profpages/merrinst/exponentiation_and_java.html 
long int_exp(long b, long n) { 	
    long temp;
    if(n & 1) 		
        return b*int_exp(b, n^1); 	
    else 		
        return n ? ( temp = int_exp(b, n>>1), temp*temp ) : 1; 
}

/*Use operator string to see which operation to perform*/
/*Operations are not safe right now we should add some additional security level*/
lval eval_op(lval x, char* op, lval y) {
 /* if either value is an error return it */
 if (x.type == LVAL_ERR) { return x; }
 if (y.type == LVAL_ERR) { return y; }

 if (0 == strcmp(op, "+")) { return lval_num(x.num + y.num); }
 if (0 == strcmp(op, "-")) { return lval_num(x.num - y.num); }
 if (0 == strcmp(op, "*")) { return lval_num(x.num * y.num); }
 if (0 == strcmp(op, "/")) {
	 return  0 == y.num
		 ? lval_err(LERR_DIV_ZERO)
		 : lval_num(x.num / y.num);
 }
 if (0 == strcmp(op, "%")) { return lval_num(x.num % y.num); }
 if (0 == strcmp(op, "^")) { return lval_num(int_exp(x.num, y.num)); } 
 return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
 /* If tagged as number return it directly. */
 if (strstr(t->tag, "number")) {

  /* Check if there is some error in conversion  */
  errno = 0;
  long x = strtol(t->contents, NULL, 10); //convert string to long
  //we use ERANGE macro to check if number out of range for long 
  return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
 }

 /* The operator is always second child.  */
 char* op = t->children[1]->contents;

 /* We store the third child in `x`  */
 lval x = eval(t->children[2]);

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
        operator : '+' | '-' | '*' | '/' | '%' | '^' ;  \
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
  lval res = eval(result.output);
  lval_println(res);
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
