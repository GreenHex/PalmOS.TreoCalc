/*
Pilot Native stubs with gcc - V3
Function names are those used by gcc whn generating code for double stuff
Currently in development.

Copyright (C) Aug 1998 François Pessaux (francois.pessaux@inria.fr)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// #include <Common.h>
// #include <System/SysAll.h>
// #define _DONT_USE_FP_TRAPS_ 1
#include <Hs.h>
#include <FloatMgr.h>


/* ************************************************************************ */
/* String <-> double management                                             */
/* ************************************************************************ */

/* Conversion Asci -> double */
/* PalmOS trap: sysFloatAToF */
/* Gcc symbol: None */
void SysTrapFlpAToF(FlpDouble*, Char*) SYS_TRAP(sysTrapFlpDispatch);

double GccSysFlpAToF( char* s )
{
    FlpCompDouble fcd;
    asm("moveq.l %0,%%d2" : : "i" (sysFloatAToF) : "d2");
    SysTrapFlpAToF(&fcd.fd, s);
    return fcd.d;
}

/* Conversion double -> Asci */
/* PalmOS trap: sysFloatFToA */
/* Gcc symbol: None */
void SysTrapFlpFToA (double, Char*) SYS_TRAP(sysTrapFlpDispatch);

void GccSysFlpFToA (double value, Char *buffer)
{
  asm("moveq.l %0,%%d2" : : "i" (sysFloatFToA) : "d2");
  SysTrapFlpFToA (value, buffer) ;
  return ;
}



/* ************************************************************************ */
/* Arithmetic double operators                                              */
/* ************************************************************************ */

/* Proto for binary functions with type double -> double -> double */
void SysTrapFlpBinaryOp(FlpDouble*, double, double)
     SYS_TRAP(sysTrapFlpEmDispatch);

/* + */
/* PalmOS trap: _d_add */
/* Gcc symbol: __adddf3 */
double __adddf3 (double d1, double d2)
{
    FlpCompDouble fcd;
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_add) : "d2");
    SysTrapFlpBinaryOp (&fcd.fd, d1, d2);
    return fcd.d;
}

/* - */
/* PalmOS trap: _d_sub */
/* Gcc symbol: __subdf3 */
double __subdf3 (double d1, double d2)
{
    FlpCompDouble fcd;
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_sub) : "d2");
    SysTrapFlpBinaryOp (&fcd.fd, d1, d2);
    return fcd.d;
}

/* * */
/* PalmOS trap: _d_mul */
/* Gcc symbol: __muldf3 */
double __muldf3 (double d1, double d2)
{
    FlpCompDouble fcd;
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_mul) : "d2");
    SysTrapFlpBinaryOp (&fcd.fd, d1, d2);
    return fcd.d;
}

/* / */
/* PalmOS trap: _d_div */
/* Gcc symbol: __divdf3 */
double __divdf3 (double d1, double d2)
{
    FlpCompDouble fcd;
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_div) : "d2");
    SysTrapFlpBinaryOp (&fcd.fd, d1, d2);
    return fcd.d;
}



/* ************************************************************************ */
/* Cast functions                                                           */
/* ************************************************************************ */

/* Conversion from double to int */
/* PalmOS trap: _d_dtoi */
/* Gcc symbol: __fixdfsi */
SDWord SysTrapFlpDoubleToInt (double) SYS_TRAP(sysTrapFlpEmDispatch);

SDWord __fixdfsi (double d)
{
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_dtoi) : "d2");
    return SysTrapFlpDoubleToInt (d) ;
}

/* Conversion from int to double */
/* PalmOS trap: _d_itod */
/* Gcc symbol: __floatsidf */
void SysTrapFlpIntToDouble (FlpDouble*, SDWord) SYS_TRAP(sysTrapFlpEmDispatch);

double __floatsidf (SDWord i)
{
    FlpCompDouble fcd;
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_itod) : "d2");
    SysTrapFlpIntToDouble (&fcd.fd, i);
    return fcd.d;
}



/* ************************************************************************ */
/* Double comparision functions                                             */
/* ************************************************************************ */

/* Proto for binary functions with type double -> double -> int */
SDWord SysTrapFlpComparOp(double, double) SYS_TRAP(sysTrapFlpEmDispatch);

/* == */
/* PalmOS trap: _d_feq */
/* Gcc symbol: __eqdf2 */
SDWord __eqdf2 (double d1, double d2)
{
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_feq) : "d2");
    return (! SysTrapFlpComparOp (d1, d2)) ;
}

/* != */
/* PalmOS trap: _d_fne */
/* Gcc symbol: __nedf2 */
SDWord __nedf2 (double d1, double d2)
{
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_fne) : "d2");
    return (SysTrapFlpComparOp (d1, d2)) ;
}

/* < */
/* PalmOS trap: _d_flt */
/* Gcc symbol: __ltdf2 */
SDWord __ltdf2 (double d1, double d2)
{
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_flt) : "d2");
    return (- SysTrapFlpComparOp (d1, d2)) ;
}

/* <= */
/* PalmOS trap: _d_fle */
/* Gcc symbol: __ledf2 */
SDWord __ledf2 (double d1, double d2)
{
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_fle) : "d2");
    return (1 - SysTrapFlpComparOp (d1, d2)) ;
}

/* > */
/* PalmOS trap: __d_fgt */
/* Gcc symbol: __gtdf2 */
SDWord __gtdf2 (double d1, double d2)
{
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_fgt) : "d2");
    return SysTrapFlpComparOp (d1, d2) ;
}

/* >= */
/* PalmOS trap: __d_fge */
/* Gcc symbol: __gedf2 */
SDWord __gedf2 (double d1, double d2)
{
    asm("moveq.l %0,%%d2" : : "i" (sysFloatEm_d_fge) : "d2");
    return (SysTrapFlpComparOp (d1, d2)) - 1 ;
}

