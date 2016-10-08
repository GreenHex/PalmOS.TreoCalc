/*
  TOKEN.H
  =======
  (c) Copyright Paul Griffiths 2002
  Email: mail@paulgriffiths.net

  Interface to tokenising operation
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <FloatMgr.h>

#ifndef PG_TOKEN_H
#define PG_TOKEN_H

/*  Macros  */ 
#define BUFFERSIZE   (256)
#define STACKSIZE    (256)

/*
#define TOK_OPERAND    (0)
#define TOK_OPERATOR   (1)

#define OP_PLUS        (0)
#define OP_MINUS       (1)
#define OP_MULTIPLY    (2)
#define OP_DIVIDE      (3)
#define OP_POWER       (4)
#define OP_MOD         (5)
#define OP_RPAREN      (6)
#define OP_LPAREN      (7)
#define OP_BAD        (99)
*/

typedef enum {
	TOK_OPERAND = 0,
	TOK_OPERATOR = 1
} token_e;

typedef enum {
	OP_PLUS = 0,
	OP_MINUS = 1,
	OP_MULTIPLY = 2,
	OP_DIVIDE = 3,
	OP_POWER = 4,
	OP_MOD = 5,
	OP_RPAREN = 6,
	OP_LPAREN = 7,
	OP_BAD = 99
} operator_e;

/*  Struct definitions  */

typedef union {
	int				operator;
	FlpDouble		operand;
} value_t;

typedef struct {
    token_e 		type;
   	value_t			value;
} token_t;

typedef struct {
    char 			symbol;
    operator_e  	value;
    int  			precedence;
} operator_t;


/*  Array of operator descriptions  */

extern operator_t oplist[];


/*  Function declarations  */

char* GetNextToken(char* input, token_t* t);


#endif  /*  PG_TOKEN_H  */
