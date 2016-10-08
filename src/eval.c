/*
  EVAL.C
  ======
  (c) Copyright Paul Griffiths 2002
  Email: mail@paulgriffiths.net

  Implementation of expression evaluation operation
*/


#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"


/*  Converts a string based infix numeric expression
    to a string based postfix numeric expression      */

char* toPostfix(char* infix, char* postfix)
{
    char			buffer[BUFFERSIZE];
    value_t			stack[STACKSIZE];
    int				top = 0;
    token_t			t;
    
    *postfix = 0;                         /*  Empty output buffer     */

    while ( (infix = GetNextToken(infix, &t)) ) {
        if ( t.type == TOK_OPERAND ) {

            /*  Always output operands immediately  */

            sprintf(buffer, "%d ", t.value.operator);
            strcat(postfix, buffer);
        }
        else {

            /*  Add the operator to the stack if:
                 - the stack is empty; OR
                 - if the operator has a higher precedence than
                   the operator currently on top of the stack; OR
                 - An opening parenthesis is on top of the stack   */

            if ( top == 0 || oplist[t.value.operator].precedence >
                             oplist[stack[top].operator].precedence ||
                             oplist[stack[top].operator].value == OP_LPAREN )
			{
                stack[++top].operator = t.value.operator;
            }
            else {
                int balparen = 0;

                /*  Otherwise, remove all operators from the stack
                    which have a higher precedence than the current
                    operator. If we encounter a closing parenthesis,
                    keep removing operators regardless of precedence
                    until we find its opening parenthesis.            */

                while ( top > 0 && ((oplist[stack[top].operator].precedence >=
                                     oplist[t.value.operator].precedence)
                                    || balparen)
                                && !(!balparen
                                     && oplist[stack[top].operator].value ==
                                        OP_LPAREN) ) {
                    if ( stack[top].operator == OP_RPAREN )
                        ++balparen;
                    else if ( stack[top].operator == OP_LPAREN )
                        --balparen;
                    else {
                        sprintf(buffer, "%c ", oplist[stack[top].operator].symbol);
                        strcat(postfix, buffer);
                    }
                    --top;
                }
                stack[++top].operator = t.value.operator;
            }
        }
    }


    /*  Output any operators still on the stack  */

    while ( top > 0 ) {
        if ( oplist[stack[top].operator].value != OP_LPAREN &&
             oplist[stack[top].operator].value != OP_RPAREN ) {
            sprintf(buffer, "%c ", oplist[stack[top].operator].symbol);
            strcat(postfix, buffer);
        }
        --top;
    }

    return postfix;
}    

/****************************************/

if ((tokStackPi == tokStackP) // stack is empty;	
	|| (tokInfixPi->value.op.type == OP_LPAREN)
	|| (tokInfixPi->value.op.precedence > (tokStackPi - 1)->value.op.precedence))
{
	*tokStackPi++ = *tokInfixPi;

	if (tokInfixPi->value.op.type == OP_LPAREN)
	{
		++numParantheses;
	}
	Alert("C ", " ", (tokStackPi - 1)->value.op.type);
}
else
{
	while ((tokStackPi != tokStackP)
			&& (tokInfixPi->value.op.precedence <= (tokStackPi - 1)->value.op.precedence)
			&& (!(tokStackPi - 1)->value.op.type == OP_LPAREN))
	{
		*tokPostfixPi++ = *tokStackPi--;
		
		if (tokStackPi->value.op.type == OP_LPAREN)
		{
			--numParantheses;
			--tokStackPi;
		}
		Alert("D ", " ", (tokStackPi - 1)->type);
	}
	Alert("B ", " ", (tokPostfixPi - 1)->type);
	*tokStackPi++ = *tokInfixPi;
}
/****************************************/

/*  Parses a postfix expression and returns its value  */

FlpDouble parsePostfix(char* postfix)
{
    token_t 		t;
    double      	stack[STACKSIZE];
    int         	top = 0;
    
    while ( (postfix = GetNextToken(postfix, &t)) ) {
        if ( t.type == TOK_OPERAND ) {
            stack[++top] = t.value.operator;       /*  Store operand on stack  */
        }
        else {
            FlpDouble a, b, value;
            
            if ( top < 2 ) {              /*  Two operands on stack?  */
                // puts("Stack empty!");
                // exit(1);
            }

            b = stack[top--];             /*  Get last two operands   */
            a = stack[top--];

            switch ( t.value.operator) {
            case OP_PLUS:                 /*  Perform operation       */
                value = a + b;
                break;
                
            case OP_MINUS:
                value = a - b;
                break;
                
            case OP_MULTIPLY:
                value = a * b;
                break;
                
            case OP_DIVIDE:
                value = a / b;
                break;

            case OP_MOD:
                //value = fmod(a, b);
                break;
                
            case OP_POWER:
                //value = pow(a, b);
                break;
                
            default:
                // printf("Bad operator: %c\n", oplist[t.value].symbol);
                // exit(1);
                break;
            }            
            stack[++top] = value;        /*  Put value back on stack  */
        }
    }
    return stack[top];
}
            

/*  Evaluates an postfix numeric expression  */

double evaluate(char * infix) {
    char postfix[BUFFERSIZE];
    return parsePostfix(toPostfix(infix, postfix));
}
