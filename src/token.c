/*
  TOKEN.C
  =======
  (c) Copyright Paul Griffiths 2002
  Email: mail@paulgriffiths.net

  Implementation of tokenising operation
*/

#include "token.h"

/*  Array of operators  */

operator_t oplist[] = { {'+', OP_PLUS,     1},
                         {'-', OP_MINUS,    1},
                         {'*', OP_MULTIPLY, 2},
                         {'/', OP_DIVIDE,   2},
                         {'^', OP_POWER,    3},
                         {'%', OP_MOD,      2},
                         {')', OP_RPAREN,   4},
                         {'(', OP_LPAREN,   5},
                         {0,   0,           0} };


/*  Gets the next token from a string based numeric
    expression. Returns the address of the first
    character after the token found.                 */

char* GetNextToken(char* input, token_t* t)
{
    while ( *input && isspace(*input) )  /*  Skip leading whitespace  */
        ++input;

    if ( *input == 0 )                   /*  Check for end of input   */
        return NULL;

    if ( isdigit(*input) )
    {             /*  Token is an operand      */
        t->type  = TOK_OPERAND;
        t->value.operand = FlpAToF(input);
    }
    else
    {                               /*  Token is an operator     */
        int n = 0, found = 0;

        t->type = TOK_OPERATOR;

        while ( !found && oplist[n].symbol ) {
            if ( oplist[n].symbol == *input ) {
                t->value.operator = oplist[n].value;
                found = 1;
            }
            ++n;
        }

        if ( !found ) {
            // printf("Bad operator: %c\n", *input);
            // exit(1);
        }
        ++input;
    }        
    return input;
}
