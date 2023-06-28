// trying out buildyourownlisp.com
// TODO
// 1. research union V
// 2. add a double type V
// 3. make it in a union and replace the type system
#include "mpc.h"
#include <editline.h>
#include <errno.h>
#include <histedit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { LVAL_INT, LVAL_ERR, LVAL_DBL };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct {
  int type;
  union {
    long num_l;
    double num_d;
  } num;
  int err;
} lval;

lval lval_ch(lval x.num) {

  return 0;
}
lval lval_int(long x) {
  lval v;
  v.type = LVAL_INT;
  v.num.num_l = x;
  return v;
}

lval lval_dbl(double x) {
  lval v;
  v.type = LVAL_DBL;
  v.num.num_d = x;
  return v;
}

lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

void lval_print(lval v) {
  switch (v.type) {
  case LVAL_INT:
    printf("%li\n", v.num.num_l);
    break;
  case LVAL_DBL:
    printf("%lf\n", v.num.num_d);
  case LVAL_ERR:
    if (v.err == LERR_DIV_ZERO) {
      printf("Error: Division by Zero!\n");
    }
    if (v.err == LERR_BAD_OP) {
      printf("Error : Bad operator!\n");
    }
    if (v.err == LERR_BAD_NUM) {
      printf("Error : Invalid number!\n");
    }
    break;
  }
}

lval eval_op(lval x, char *op, lval y) {
  if (x.type == LVAL_ERR) {
    return x;
  }
  if (y.type == LVAL_ERR) {
    return y;
  }
    if (strcmp(op, "+") == 0) {
    return lval_ch(x.num + y.num);
  }/*

  if (strcmp(op, "-") == 0) {
    return lval_num(x.num - y.num);
  }
  if (strcmp(op, "*") == 0) {
    return lval_num(x.num * y.num);
  }
  if (strcmp(op, "%") == 0) {
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num % y.num);
  }
  if (strcmp(op, "/") == 0) {
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  }
  */
  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t *t) {
  if (strstr(t->tag, "number")) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_int(x) : lval_err(LERR_BAD_NUM);
  }
  char *op = t->children[1]->contents;
  //  printf("%s\n",op);
  lval x = eval(t->children[2]);
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  return x;
}

int main(int argc, char **argv) {

  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
            "                                                   \
            number   : /-?[0-9]+/ | /-?[0-9]+/./-?[0-9]+/  ;    \
            operator : '+' | '-' | '*' | '/' | '%' ;            \
            expr     : <number> | '(' <operator> <expr>+ ')' ;  \
            lispy    : /^/ <operator> <expr>+ /$/ ;             \
            ",
            Number, Operator, Expr, Lispy);

  puts("Lispy v0.0.0.0.0.1 beta");
  puts("press Ctrl+c to exit\n");

  while (1) {
    char *input = readline("lispy>");
    add_history(input);
    mpc_result_t res;
    if (mpc_parse("<stdin>", input, Lispy, &res)) {
      lval result = eval(res.output);
      lval_print(result);
      mpc_ast_delete(res.output);
    } else {
      mpc_err_print(res.error);
      mpc_err_delete(res.error);
    }
    free(input);
  }
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  return 0;
}
