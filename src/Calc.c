/*
 * Calc.c
 */

#include "Calc.h"

// Prototypes
static Char* 				GetNextToken(Char* inStr, token_t* t, calcError_e* errVal, operator_t* opList);
static calcError_e 			tokenize(Char* inputStr, token_t* t, operator_t* opList);
static calcError_e 			InfixToPostfix(Char* inputStr, token_t*	tokPostfixP);
static calcError_e 			evalPostfix(token_t* t, Char* resultStr, UInt8 precision);
static void 				formatResultStr(FlpDouble fdbl, Char* str, UInt8 precision);

static Char* GetNextToken(Char* inStr, token_t* t, calcError_e* errVal, operator_t* opList)
{                     	
	*errVal = noError;
	
	t->type = TOK_INVALID;
		
    while (*inStr && TxtCharIsSpace(*inStr))  /*  Skip leading whitespace  */
        ++inStr;
         
    if (*inStr == 0)                   /*  Check for end of input   */
        return NULL;

    if (TxtCharIsDigit(*inStr)
    		|| (*inStr == '.'))  /* operand */
    {
    	Char*			numStr;
    	UInt16			i = 0;
    	UInt16			numDecimals = 0;
    	
    	numStr = MemPtrNew(MAX_NUM_STR_SIZE);
    	MemSet(numStr, MAX_NUM_STR_SIZE, 0);
    	
    	t->type = TOK_OPERAND;
    	
    	while (TxtCharIsDigit(*inStr)
    		|| (*inStr == '.'))             
        {
        	if (*inStr == '.')
        	{
        		++numDecimals;
        	}	
        	numStr[i++] = *inStr++;
        }

		if (numDecimals > 1)
		{
			*errVal = errInvalidNumberString;
		}
		else if (StrLen(numStr) > 10)
		{
			*errVal = errNumberOverflow;	
		}
		else
		{
			FlpCompDouble		cdbl;
			cdbl.d = GccSysFlpAToF(numStr);
			t->value.num = cdbl.fd;
			// FlpBufferAToF(&(t->value.num), numStr);			
		}	
			
		if (numStr)
			MemPtrFree(numStr);
			
    }
    else /* operator */
    {
    	UInt16		i = 0;                               
 
        t->type = TOK_OPERATOR;
 
		t->value.op = opList[0];

		while (opList[i].symbol)
        {
        	if (opList[i].symbol == *inStr)
        	{
        		t->value.op = opList[i];

        		break;	
        	}
        	++i;	
        }

        ++inStr;

		if (t->value.op.type == OP_UNKNOWN)
		{
			*errVal = errOperatorUnknown;
		}	
    }        
    return inStr;
    
} // getNextToken

/*
 * tokenize
 */
static calcError_e tokenize(Char* inputStr, token_t* t, operator_t* opList)
{
	calcError_e 		errVal = noError;
	Char*				strP;
	
	strP = inputStr;
	
	while ((strP) && (errVal == noError))
	{
		strP = GetNextToken(strP, t++, &errVal, opList);
	}
		
	return errVal;
	
} // tokenize

/*
 * InfixToPostfix
 */
static calcError_e InfixToPostfix(Char* inputStr, token_t*	tokPostfixP)
{
	operator_t opList[] = { {'?', OP_UNKNOWN,  0},		/* 0 */
						{'+', OP_PLUS,     1},		/* 1 */
                    	{'-', OP_MINUS,    1},		/* 2 */
                    	{'*', OP_MULTIPLY, 2},		/* 3 */
                    	{'/', OP_DIVIDE,   2},		/* 4 */
                    	{'^', OP_POWER,    3},		/* 5 */
                    	{'%', OP_MOD,      3},		/* 6 */
                    	{')', OP_RPAREN,   0},		/* 7 */
                    	{'(', OP_LPAREN,   0},		/* 8 */
						{0,   0,           0} };	/* 9 */
	
	calcError_e			errVal = noError;
	token_t*			tokInfixP = NULL;
	
	tokInfixP = MemPtrNew(sizeof(token_t) * MAX_TOKENS);
	
	if (tokInfixP)
	{
		MemSet(tokInfixP, sizeof(token_t) * MAX_TOKENS, 0);
		
		errVal = tokenize(inputStr, tokInfixP, opList);
			
		if (errVal == noError)
		{
			token_t*			tokInfixPi = tokInfixP;

			if ((tokInfixPi->type == TOK_OPERATOR)
					&& (!((tokInfixPi->value.op.type == OP_MINUS)
						|| (tokInfixPi->value.op.type == OP_PLUS)
					 	|| (tokInfixPi->value.op.type == OP_LPAREN))))
			{
				errVal = errInvalidExpression;
			}
			else
			{
				token_t*			tokStackP = NULL;
				
				tokStackP = MemPtrNew(sizeof(token_t) * STACK_SIZE);
				
				if (tokStackP)
				{				
					token_t				lastToken;
					token_t 			tmpToken;
					
					token_t*			tokPostfixPi = tokPostfixP;
					token_t*			tokStackPi = tokStackP;
					
					MemSet(tokStackP, sizeof(token_t) * STACK_SIZE, 0);
				
					lastToken.type = TOK_INVALID;
					
					while (tokInfixPi->type)
					{
						if (((tokInfixPi->type == TOK_OPERATOR)
								&& (tokInfixPi->value.op.type == OP_LPAREN))
							&& (lastToken.type == TOK_OPERAND))
						{
							tmpToken.type = TOK_OPERATOR;
							tmpToken.value.op = opList[OP_MULTIPLY];
							*++tokStackPi = tmpToken;
						}
						else if ((tokInfixPi->type == TOK_OPERAND)
								&& ((lastToken.type == TOK_OPERATOR)
							&& (lastToken.value.op.type == OP_RPAREN)))
						{
							tmpToken.type = TOK_OPERATOR;
							tmpToken.value.op = opList[OP_MULTIPLY];
							*++tokStackPi = tmpToken;
						}
						else if (((tokInfixPi->type == TOK_OPERATOR)
								&& ((tokInfixPi->value.op.type == OP_MINUS)
									|| (tokInfixPi->value.op.type == OP_PLUS)))
							&& (lastToken.type != TOK_OPERAND))
						{
							FlpCompDouble 		cdbl;
							
							cdbl.d = 0;
							
							tmpToken.type = TOK_OPERAND;
							tmpToken.value.num = cdbl.fd;
							*tokPostfixPi++ = tmpToken;
						}
						
						lastToken = *tokInfixPi;
						
						if (tokInfixPi->type == TOK_OPERAND)
						{
							*tokPostfixPi++ = *tokInfixPi++;
						}
						else if (tokInfixPi->type == TOK_OPERATOR)
						{
							if (tokStackPi == tokStackP)
							{
								*++tokStackPi = *tokInfixPi++; // leave stack bottom...
								//Alert("A.", "Empty Stack", tokStackPi->value.op.type);
							}
							else if (tokInfixPi->value.op.type == OP_LPAREN)
							{
								*++tokStackPi = *tokInfixPi++;
								 //Alert("B.", "Open Brackets", tokStackPi->value.op.type);						
							} 
							else if (tokInfixPi->value.op.type == OP_RPAREN)
							{
								//Alert("C.", "Close Brackets", 0);
								
								while ((tokStackPi != tokStackP)
										&& (tokStackPi->value.op.type != OP_LPAREN))
								{
									*tokPostfixPi++ = *tokStackPi--;
									//Alert("C.1.", "Pop Stack", tokPostfixPi->value.op.type);	
								}
								if (tokStackPi != tokStackP)
									--tokStackPi;
								++tokInfixPi;								
							}
							else if (tokInfixPi->value.op.precedence <= tokStackPi->value.op.precedence)
							{
								//Alert("D.", "Lower Precedence", 0);
								while ((tokStackPi != tokStackP)
										&& (tokInfixPi->value.op.precedence <= tokStackPi->value.op.precedence))
								{
									*tokPostfixPi++ = *tokStackPi--;
									//Alert("D.1", "Pop Stack", (tokPostfixPi-1)->value.op.type);	
								}
								*++tokStackPi = *tokInfixPi++;
							}
							else
							{
								*++tokStackPi = *tokInfixPi++;
								//Alert("E.", "Push to Stack", tokStackPi->value.op.type);
							}
						}
					}
					while (tokStackPi != tokStackP) // pop remaining operands
					{
						*tokPostfixPi++ = *tokStackPi--;
						//Alert("F.", "Pop Stack", (tokPostfixPi-1)->value.op.type);	
					}					
				}
				else
				{
					errVal = errMemory;
				}

				if (tokStackP)
					MemPtrFree(tokStackP);
			
				
			}		
		}	
		MemPtrFree(tokInfixP);
	}
	else
	{
		errVal = errMemory;
	}
				
	return errVal;
	
} // InfixToPostfix

/*
 * evalPostfix
 */
static calcError_e evalPostfix(token_t* t, Char* resultStr, UInt8 precision)
{
	calcError_e			errVal = noError;
	
	token_t*			s = NULL;
				
	s = MemPtrNew(sizeof(token_t) * STACK_SIZE);
	
	if (s)
	{				
		token_t*			ti = t;
		token_t*			si = s;
		
		MemSet(s, sizeof(token_t) * STACK_SIZE, 0);
		
		while (ti->type)
		{
			if (ti->type == TOK_OPERAND)
			{
				*(++si) = *ti; // bottom of stack is empty
			}
			else if (ti->type == TOK_OPERATOR)
			{
				FlpCompDouble		a;
				FlpCompDouble		b;
				FlpCompDouble		result;
				
				if ((si - s) < 2)
				{
					errVal = errInvalidExpression;
					break;
				}
				
				a.fd = (si--)->value.num;
				b.fd = (si--)->value.num;
				
				/*
				FlpFToA(a.fd, resultStr);
				Alert(resultStr, "   A ", ti->value.op.type);
				FlpFToA(b.fd, resultStr);
				Alert(resultStr, "    B ", ti->value.op.type);
				*/
				// Alert("Operand ", "", ti->value.op.type);
				
				switch (ti->value.op.type)
				{
					case OP_PLUS:
					
						FlpBufferCorrectedAdd(&(result.fd), b.fd, a.fd, 0);
						// result.d = b.d + a.d;
						//FlpFToA(result.fd, resultStr);
						//Alert("Operand + ", resultStr, ti->value.op.type);
						break;
						
					case OP_MINUS:
					
						FlpBufferCorrectedSub(&(result.fd), b.fd, a.fd, 0);
						// result.d = b.d - a.d;
						//FlpFToA(result.fd, resultStr);
						//Alert("Operand - ", resultStr, ti->value.op.type);
						break;
						
					case OP_MULTIPLY:
						
						result.d = __muldf3(b.d, a.d);
						// result.fd = _d_mul(b.fd, a.fd);
				 		// result.d = b.d * a.d;
						
						break;
						
					case OP_DIVIDE:

						result.d = __divdf3(b.d, a.d);
						// result.d =  b.d / a.d;
						// Alert("Operand ", "", ti->value.op.type);
						break;
						
					case OP_POWER:
						
						break;
						
					default:
					
						break;	
				}
				
				(++si)->value.num = (FlpDouble)(result.fd);

				si->type = TOK_OPERAND;	
			}	
			++ti;
		}
		
		formatResultStr(si->value.num, resultStr, precision);
		/*
		{
			token_t*			tokPostfixPi = t;			
			
			while (tokPostfixPi->type)
			{
				if (tokPostfixPi->type == TOK_OPERAND)
				{
					Char		str[100];
					
					FlpFToA(tokPostfixPi->value.num, str);
					
					Alert("Operand ", str, tokPostfixPi->type);
				}
				else if (tokPostfixPi->type == TOK_OPERATOR)	
				{
					Alert("Operator", " ", tokPostfixPi->value.op.type);
				}
				
				++tokPostfixPi;
			}
		} */
		MemPtrFree(s);
	}
	
	return errVal;
	
} // evalPostfix

/*
 * formatResultStr
 */
static void formatResultStr(FlpDouble fdbl, Char* str, UInt8 precision)
{
	FlpCompDouble		cdbl;
	
	UInt32				mantissa = 0;
	Int16				exponent = 0; // FlpGetExponent(fdbl);
	Int16				sign = 0;
	
	FlpBase10Info(fdbl, &mantissa, &exponent, &sign);
	 
	cdbl.fd = fdbl;
	GccSysFlpFToA(cdbl.d, str);

	exponent = (exponent & 0x000F) - 8;
	
	if ((exponent > 0) && (exponent < 8))
	{
		unsigned char 		c, *d = str;
		Int16 				i = 0;
		Boolean				bDecimal = false;

		while ((c = *str++))
		{
			if (TxtCharIsDigit(c))
			{
				*d++ = c;
			}
			else if (c == '-')
			{
				*d++ = c;
				--i;
			}
			else if ((c == 'e') || (c == 'E'))
			{
				break;
			}
			
			if (bDecimal)
			{
				--precision;
			}
			
			if (i == exponent)
			{
				*d++ = '.';
				bDecimal = true;
			}
			
			if ((!precision) && (bDecimal)) break;
						
			++i;
		}				
		*d = 0;	
	}
		
} // formatResultStr

/*
 * calculate
 */
Boolean calculate(Char* inputStr, Char* resultStr, UInt8 precision)
{
	Boolean 			retVal = false;
	calcError_e			errVal = noError;
	
	token_t*			tokPostfixP = NULL;

	tokPostfixP = MemPtrNew(sizeof(token_t) * MAX_TOKENS);
	
	if (tokPostfixP)
	{
		MemSet(tokPostfixP, sizeof(token_t) * MAX_TOKENS, 0);
	
		errVal = InfixToPostfix(inputStr, tokPostfixP);
		
		if(errVal == noError)
		{
			errVal = evalPostfix(tokPostfixP, resultStr, precision);
		}
	
		if (errVal != noError)
		{
			StrCopy(resultStr, "Expression Error");
		}
		else
		{
			retVal = true;
		}
			
		MemPtrFree(tokPostfixP);
	}
	return retVal;	

} // calculate

/*
 * Calc.c
 */