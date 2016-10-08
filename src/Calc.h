/*
 * Calc.h
 */

#ifndef __CALC_H__
#define __CALC_H__

#include <Hs.h>
#include <FloatMgr.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Common.h"
#include "PhoneUtils.h"

#define MAX_TOKENS				256
#define STACK_SIZE				128
#define MAX_NUM_STR_SIZE		64

typedef enum {
	noError 					= 0,
	errMemory 					= 1,
	errInvalidNumberString 		= 2,
	errNumberOverflow			= 3,
	errOperatorUnknown 			= 4,
	errInvalidExpression 		= 5	
} calcError_e;

typedef enum {
	TOK_INVALID					= 0,
	TOK_OPERATOR 				= 1,
	TOK_OPERAND 				= 2
} token_e;

typedef enum {
	OP_UNKNOWN 					= 0,
	OP_PLUS 					= 1,
	OP_MINUS 					= 2,
	OP_MULTIPLY 				= 3,
	OP_DIVIDE 					= 4,
	OP_POWER 					= 5,
	OP_MOD 						= 6,
	OP_RPAREN 					= 7,
	OP_LPAREN 					= 8,
	OP_BAD 						= 99
} operator_e;

/*
 * Struct definitions
 */
typedef struct {
    Char 						symbol;
    operator_e  				type;
    UInt8  						precedence;
} operator_t;

typedef union {
	operator_t					op;
	FlpDouble					num;
} value_t;

typedef struct {
    token_e 					type;
   	value_t						value;
} token_t;

// gccfloats.c
extern double 					GccSysFlpAToF(char* s);
extern void 					GccSysFlpFToA(double value, Char *buffer);
extern double 					__muldf3(double d1, double d2);
extern double 					__divdf3(double d1, double d2);

// prototypes
extern Boolean 					calculate(Char* inputStr, Char* resultStr, UInt8 precision);

#endif /* __CALC_H__ */

/*
 * Calc.h
 */