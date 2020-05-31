#include "common.h"
/* $Header: predef.c,v 3.2 90/04/25 16:24:06 gary Locked $ */
#include "manager.h"
#include "predef.h"
#include "micro.h"
#include "symbol.h"

#ifdef TURBOC
#include <io.h>
#endif


/* forward static declaration */
#ifdef Prototype
void Mk_Pd(C_Str str, C_Pd (c), int x);

#else
static void Mk_Pd();

#endif


IPtr     mcPdCall(func, args)
    IPtr     func,
            args;
{
    IPtr     i,
            j;

    if (Pd_argc(func) >= 0 && mcLen(args) != Pd_argc(func)) {
	MSG("Error", Pd_name(func));
	RT_ERROR("Wrong number of args");
    }
    switch (Pd_argc(func)) {
      case 0:
	return (*Pd_Code(func)) ();
      case 1:
	Pd_Arg(i);
	return (*Pd_Code(func)) (i);
      case 2:
	Pd_Arg(i);
	Pd_Arg(j);
	return (*Pd_Code(func)) (i, j);
      default:
	return (*Pd_Code(func)) (args);
    }
}

struct {
    char   *pd_name;
    C_Pd    (pd_code);
    short   ac;
}

        PdList[] = {
#ifdef GRAPHICS
   "delay",        pdDelay,       1,
   "XtoYratio",    pdXtoYratio,   0,
   "where_am_I",   pdWhere,       0,
   "clear_screen", pdClearScreen, 0,
   "rectangle",    pdRectangle,   -1,
   "new_object",   pdNewObj, 0,
   "add_object",   pdAddObj, 1,
   "del_object",   pdDelObj, 1,
   "resolution",   pdRes, 0,
   "title_window", pdTitleW,1,
   "thickline",    pdThickLine,1,
   "graphics", pdGraphics, 1,
   "scale",    pdScale,    4,
   "move",     pdMove,     2,
   "draw",     pdDraw,     2,
   "textout",  pdTextout,  -1,
   "get_coord", pdGetCoord,0,
   "erasable",  pdErasable, -1,
   "char_mult", pdCharMult, 1,
   "point",     pdPoint,   -1,
   "save_graph", pdSaveGraph, 1,
   "load_graph", pdLoadGraph, 1,
#endif
   "as_string", pdAsString, -1,
	    "system", pdSys, -1,
	    "lo", pdLo, 1,
	    "hi", pdHi, 1,
	    "origin", pdOrigin, -1,
    "abs", pdAbs, 1,
    "arb", pdRandom, 1,
    "ceil", pdCeil, 1,
    "char", pdChar, 1,
    "domain", pdDomain, 1,
    "even", pdEven, 1,
    "fix", pdFix, 1,
    "float", pdFloat, 1,
    "floor", pdFloor, 1,
    "is_atom", pdAtom, 1,
    "is_boolean", pdBool, 1,
    "is_defined", pdDefined, 1,
    "is_integer", pdInt, 1,
    "is_file", pdFile, 1,
    "is_floating", pdReal, 1,
    "is_rational", pdRational, 1,
    "is_func", pdFunc, 1,
    "is_map", pdMap, 1,
    "is_number", pdNumber, 1,
    "is_om", pdOm, 1,
    "is_set", pdSet, 1,
    "is_string", pdStr, 1,
    "is_tuple", pdTuple, 1,
    "max", pdMax, 2,
    "max_line", pdMaxLine, 1,
    "min", pdMin, 2,
    "npow", pdNpow, 2,
    "odd", pdOdd, 1,
    "ord", pdOrd, 1,
    "pow", pdPow, 1,
    "precision", pdPrec, 1,
    "random", pdRandom, 1,
    "image", pdRange, 1,
    "sgn", pdSign, 1,
#ifdef Screen
#ifdef TURBOC
    "video", pdVideo, 1,
#endif
#endif
    "eof", pdEof, 1,
    "openr", pdOpenr, 1,
    "openw", pdOpenw, 1,
    "opena", pdOpena, 1,
    "close", pdClose, 1,
    "sqrt", pdSqrt, 1,

    "randomize", pdSeed, -1,
    "log", pdLog, 1,
    "exp", pdExp, 1,
    "ln", pdLn, 1,
    "sin", pdSin, 1,
    "cos", pdCos, 1,
    "tan", pdTan, 1,
    "asin", pdASin, 1,
    "acos", pdACos, 1,
    "atan", pdATan, 1,
    "sec",  pdSec, 1,	
    "csc",  pdCsc, 1,	
    "cot",  pdCot, 1,
    "asec", pdASec, 1,
    "acsc", pdACsc, 1,
    "acot", pdACot, 1,
    "sinh", pdSinh, 1,
    "cosh", pdCosh, 1,
    "tanh", pdTanh, 1,
    "asinh", pdAsinh, 1,
    "acosh", pdAcosh, 1,
    "atanh", pdAtanh, 1,

    "den", pdDenominator, 1,
    "num", pdNumerator, 1,
    NULL, NULL
};

/* Must be called before any predefined functions are invoked */
/* Initializes the global Environment */
void    Init_Pd()
{
    int     i;

    Environment = Nil;
    for (i = 0; PdList[i].pd_name != NULL; i++) {
	Mk_Pd(PdList[i].pd_name, PdList[i].pd_code, PdList[i].ac);
    }
}



/*
 * Adds the predefined function str to the global environment, with code
 * c.
 */
static void    Mk_Pd(str, c, x) C_Str str; C_Pd (c); short x; {
    ENTER;
    Reg     (new);

    Environment = mcCons(Nil, Environment);
    Info(Environment) = New_Block(Assoc, 0);
    Var_Index(Info(Environment)) = find_id(str);
    Lock(str);

    R(new) = New_Block(Predef, 0);
    Pd_Code(R(new)) = c;
    Pd_argc(R(new)) = x;
    Pd_name(R(new)) = str;
    Var_Value(Info(Environment)) = R(new);
    LEAVE;
}


Pd1(pdAbs)
{
    switch (Kind(x)) {
      case Integer:
	return New_Integer((WORKING) (Int_Value(x) >= 0
				      ? Int_Value(x)
				      : -Int_Value(x)));
      case Bignum:{
	    ENTER;
	    Reg     (new);

	    R(new) = New_MP(Length(x));
	    mpcpy(R(new), x);
	    Sign(R(new)) = 1;
	    LEAVE   R(new);
	}
      case Rational:{
	    ENTER;
	    Reg     (new);

	    R(new) = New_MP(Length(Num(x)));
	    mpcpy(R(new), Num(x));
	    Sign(R(new)) = 1;
	    R(new) =   New_Rational(R(new),Den(x));
	    LEAVE R(new);
	}


      case Real:
	return New_Real(fabs(Real_Value(x)));
      case String:
	if (mcLen(x) != 1)
	    return OM;
	else
	    return New_Integer((WORKING) Str_Value(x)[0]);
      default:
	RT_ERROR("abs");
    }
    return OM;
}

Pd2(pdMax)
{
    if (mcLt(a, b))
	return b;
    else
	return a;
}

Pd1(pdMaxLine)
{
    int old;

    if (Kind(x) != Integer) {
	RT_ERROR("max_line: needs integer");
    }
    old = mcSetMaxLine(Int_Value(x));
    return New_Integer((WORKING)old);
}

#ifdef Screen
#ifdef TURBOC
Pd1(pdVideo)
{
    extern int directvideo;

    if (Kind(x) != Boolean) {
	RT_ERROR("video: needs boolean");
    }
    directvideo = (x==ITRUE ? 1 : 0);
    return NonPrint;
}
#endif
#endif

Pd1(pdPrec)
{   int old = r_prec;
    if (Kind(x) != Integer) {
	RT_ERROR("precision: needs Integer");
    }
    r_prec = Int_Value(x);
    return New_Integer(old);
}

Pd2(pdMin)
{
    if (mcLt(a, b))
	return a;
    else
	return b;
}


Pd1(pdAtom)
{
    return New_Boolean(Kind(x) == Atom);
}

Pd1(pdBool)
{
    return New_Boolean(Kind(x) == Boolean);
}

Pd1(pdInt)
{
    return New_Boolean(Kind(x) == Integer || Kind(x) == Bignum);
}

Pd1(pdNumber)
{
    return New_Boolean(Kind(x) == Integer
		       || Kind(x) == Bignum
		       || Kind(x) == Rational
		       || Kind(x) == Real);
}

Pd1(pdFile)
{
    return New_Boolean(Kind(x) == File && File_Value(x) != FNULL);
}

Pd1(pdFunc)
{
    return New_Boolean(Kind(x) == Closure || Kind(x) == Predef);
}

Pd1(pdOm)
{
    return New_Boolean(Kind(x)==Undefined);
}

Pd1(pdDefined)
{
    return New_Boolean(Kind(x)!=Undefined);
}

Pd1(pdReal)
{
    return New_Boolean(Kind(x) == Real);
}

Pd1(pdRational)
{
    return New_Boolean(Kind(x) == Rational
		       || Kind(x)==Integer || Kind(x)==Bignum );
}

Pd1(pdMap)
{
    ENTER;
    Reg     (ans);
    Reg     (list);
    Reg     (item);

    if (Kind(x) != Set) {
	R(ans) = IFALSE;
    } else {			       /* each element of x must be a pair */
	R(ans) = ITRUE;
	R(list) = mcStart(Set_Value(x));
	while (R(ans) == ITRUE && mcNext(&R(item), &R(list), true)) {
	    if (Kind(R(item)) != Tuple
		|| mcLen(R(item)) != 2
		|| mcAccess(Tuple_Value(R(item)), (Idx) 0) == OM)
		R(ans) = IFALSE;
	}
    }

    LEAVE   R(ans);
}

Pd1(pdSet)
{
    return New_Boolean(Kind(x) == Set);
}

Pd1(pdStr)
{
    return New_Boolean(Kind(x) == String);
}

Pd1(pdTuple)
{
    return New_Boolean(Kind(x) == Tuple);
}




Pd1(pdFloat)
{
    switch (Kind(x)) {
      case Integer:
      case Bignum:
      case Rational:
	return mpFloat(x);
      case Real:
	return x;
      default:
	RT_ERROR("float");
    }
}

Pd1(pdFix)
{
    switch (Kind(x)) {
      case Integer:
	return x;
      case Bignum:
	return x;
      case Rational: {
	ENTER;
	Reg(ans); Reg(temp);
	mpDivMod(Num(x),Den(x), &R(ans), &R(temp));
	if( Kind(R(temp))==Integer && Int_Value(R(temp))==0 ) {
	    /* OK, exact division.  Do nothing */
	} else {
	    if( Kind(R(ans))==Bignum && Sign(R(ans))==-1 )
		R(ans) = mpAdd(R(ans), BigOne);
	    else if( Kind(R(ans))==Integer && Int_Value(R(ans))<0 )
		R(ans) = New_Integer((WORKING) Int_Value(R(ans))+1);
	}
	LEAVE R(ans);
      }
	
      case Real:
	return mpFix(Real_Value(x));
      default:
        RT_ERROR("fix");
    }
}

Pd1(pdFloor)
{
    switch (Kind(x)) {
      case Bignum:
      case Integer:
	return x;
      case Rational: {
	ENTER;
	Reg(ans); Reg(temp);
	mpDivMod(Num(x),Den(x), &R(ans), &R(temp));
	LEAVE R(ans);
      }
	
      case Real:
	return mpFix(floor(Real_Value(x)));
      default:
        RT_ERROR("floor");
    }
}

Pd1(pdCeil)
{
    switch (Kind(x)) {
      case Bignum:
      case Integer:
	return x;
      case Rational: {
	ENTER;
	Reg(ans); Reg(temp);
	mpDivMod(Num(x),Den(x), &R(ans), &R(temp));
	if( Kind(R(temp))==Integer && Int_Value(R(temp))==0 ) {
	    /* OK, exact division.  Do nothing */
	} else {
	    if( Kind(R(ans))==Bignum )
		R(ans) = mpAdd(R(ans), BigOne);
	    else
		R(ans) = New_Integer((WORKING) Int_Value(R(ans))+1);
	}
	LEAVE R(ans);
      }
	
      case Real:
	return mpFix(ceil(Real_Value(x)));
      default:
        RT_ERROR("ceil");
    }
}

Pd1(pdEven)
{
    switch (Kind(x)) {
      case Bignum:{
	    ENTER;
	    Reg     (q);
	    Reg     (r);

	    mpDivMod(x, BigTwo, &R(q), &R(r));
	    R(q) = New_Boolean(Int_Value(R(r)) == 0);
	    LEAVE   R(q);
	}
      case Integer:
	return New_Boolean(0 == (Int_Value(x) % 2));
      default:
        RT_ERROR("even");
    }
}

Pd1(pdOdd)
{
    switch (Kind(x)) {
      case Bignum:{
	    ENTER;
	    Reg     (q);
	    Reg     (r);

	    mpDivMod(x, BigTwo, &R(q), &R(r));
	    R(q) = New_Boolean(Int_Value(R(r)) == 1);
	    LEAVE   R(q);
	}

      case Integer:
	return New_Boolean(0 != (Int_Value(x) % 2));
      default:
        RT_ERROR("odd");
    }
}

Pd1(pdOrd)
{
    switch (Kind(x)) {
      case String:
	if (mcLen(x) != 1)
	    return OM;
	else
	    return New_Integer((WORKING) Str_Value(x)[0]);
      default:
        RT_ERROR("ord");
    }
}

/* returns real number 0 <= rrand() < 1 */
double  rrand()
{

#ifdef Rand
    extern int rand(NONE);

    return (double) rand() / (1 << 14) / 2;
#endif

#ifdef Random
    extern long random();

    return (double) random() / (1 << 30) / 2;
#endif

#ifdef BigRand
    return (double) rand() / (1 << 30) / 2;
#endif
}

Pd(pdSeed)
{
    int     seed;
    IPtr     x = Info(args);

    switch (Kind(x)) {
      case Integer:
	seed = Int_Value(x);
	break;
      default:
	seed = 0;
    }

#ifdef Random
    srandom(seed);
#endif

#ifdef Rand
    srand(seed);
#endif

#ifdef BigRand
    srand(seed);
#endif

    return NonPrint;
}

Pd1(pdRandom)
{
    switch (Kind(x)) {
      case Bignum:{
	    ENTER;
	    Reg     (source);
	    Reg     (temp);
	    Reg     (q);
	    Reg     (r);
	    int     i;

	    if (Sign(x) == -1) {
		LEAVE   OM;
	    }
	    R(source) = New_MP(Length(x) + 2);
	    R(temp) = mpAdd(x, BigOne);
	    for (i = Length(R(source)) - 1; i >= 0; i--) {
		Digits(R(source))[i] = BASE * rrand();
	    }
	    Sign(R(source)) = 1;
	    mpDivMod(R(source), R(temp), &R(q), &R(r));
	    LEAVE   R(r);
	}
      case Integer:
	if (Int_Value(x) < 0)
	    return OM;
	return New_Integer((WORKING) ((Int_Value(x) + 1) * rrand()));
      case Rational: {
	  C_Real upper = Real_Value(mpFloat(x));
	  if (upper < 0)
	      return OM;
	  return New_Real(upper * rrand());
      }

	
      case Real:
	if (Real_Value(x) < 0)
	    return OM;
	return New_Real(Real_Value(x) * rrand());
      case String:{
	    int     i = mcLen(x);
	    int     r = (int) (rrand() * i);

	    if (i == 0)
		return OM;
	    else {
		char    c[2];

		c[0] = Str_Value(x)[r];
		c[1] = EOS;
		return New_String(c);
	    }
	}

      case Tuple:{
	    Idx     i = mcLen(x);
	    Idx     r = rrand() * i;
	    IPtr     p = Tuple_Value(x);

	    if (i == 0)
		return OM;
	    else
		return mcAccess(p, r);
	}

      case Set:{
	    return mcHAny(Set_Value(x));
	}

      default:
        RT_ERROR("random");
    }
}

Pd1(pdChar)
{
    switch (Kind(x)) {
      case Integer:{
	    char    c[2];
	    int     val = Int_Value(x);

	    if (val < 1 || val > 255)
		return OM;
	    c[0] = (char) val;
	    c[1] = EOS;
	    return New_String(c);
	}
      default:
        RT_ERROR("char");
    }
}

static IPtr     Sdom(PTR);
static IPtr Sdom(S)
    IPtr     S;
{
    ENTER;
    Reg     (ans);
    Reg     (list);
    Reg     (item);

    R(ans) = NilHT;
    /* for each pair in S, add first element to R(ans) */
    R(list) = mcStart(S);
    while (mcNext(&R(item), &R(list), true)) {
	if (Kind(R(item)) != Tuple || mcLen(R(item)) != 2) {
	    R(ans) = OM;
	    break;
	}
	R(ans) = mcHWith(mcAccess(Tuple_Value(R(item)), (Idx) 0), R(ans));
    }

    if (R(ans) != OM)
	R(ans) = New_Set(R(ans));
    LEAVE   R(ans);
}

static IPtr     Tdom(PTR);
static IPtr Tdom(T)
    IPtr     T;
{
    ENTER;
    Reg     (ans);
    Reg     (list);
    Reg     (item);
    Reg     (indx);
    Idx     i;

    i = Tuple_Origin(T);
    T = Tuple_Value(T);
    R(ans) = NilHT;
    R(list) = mcStart(T);
    while (mcNext(&R(item), &R(list), true)) {
	if (R(item) != OM) {
	    R(indx) = New_Integer((WORKING) i);
	    R(ans) = mcHWith(R(indx), R(ans));
	}
	i++;
    }

    R(ans) = New_Set(R(ans));
    LEAVE   R(ans);
}

Pd1(pdDomain)
{
    switch (Kind(x)) {
      case Set:
	return Sdom(Set_Value(x));
      case Tuple:
	return Tdom(x);
      default:
        RT_ERROR("domain");
    }
}

static IPtr     Srange(PTR);
static IPtr Srange(S)
    IPtr     S;
{
    ENTER;
    Reg     (ans);
    Reg     (list);
    Reg     (item);

    R(ans) = NilHT;
    /* for each pair in S, add second element to R(ans) */
    R(list) = mcStart(S);
    while (mcNext(&R(item), &R(list), true)) {
	if (Kind(R(item)) != Tuple || mcLen(R(item)) != 2
	    || mcAccess(Tuple_Value(R(item)), (Idx) 0) == OM) {
	    R(ans) = OM;
	    break;
	}
	R(ans) = mcHWith(mcAccess(Tuple_Value(R(item)), (Idx) 1), R(ans));
    }

    if (R(ans) != OM)
	R(ans) = New_Set(R(ans));
    LEAVE   R(ans);
}

static IPtr     Trange(PTR);
static IPtr Trange(T)
    IPtr     T;
{
    ENTER;
    Reg     (ans);
    Reg     (list);
    Reg     (next);

    R(ans) = Nil;
    R(list) = mcStart(T);
    while (mcNext(&R(next), &R(list), true)) {
	if (R(next) != OM) {
	    R(ans) = mcHWith(R(next), R(ans));
	}
    }

    R(ans) = New_Set(R(ans));
    LEAVE   R(ans);
}

Pd1(pdRange)
{
    switch (Kind(x)) {
      case Set:
	return Srange(Set_Value(x));
      case Tuple:
	return Trange(Tuple_Value(x));
      default:
        RT_ERROR("image");
    }
}

static IPtr     powset(PTR);
static IPtr powset(S)
    IPtr     S;
{
    ENTER;
    Reg     (ans);
    Reg     (copy);
    Reg     (list);
    Reg     (item);
    IPtr     i;

    R(ans) = mcCons(NilHT, Nil);

    /*
     * for each i in S, duplicate elements of ans and add i to one
     * copy
     */
    R(list) = mcStart(Set_Value(S));
    while (mcNext(&R(item), &R(list), false)) {
	R(copy) = Nil;
	for (; R(ans) != Nil; R(ans) = Next(R(ans))) {
	    /* Set from R(ans) */
	    R(copy) = mcCons(Info(R(ans)), R(copy));
	    /* Set from R(ans) with R(item) */
	    R(copy) = mcCons(Nil, R(copy));
	    Info(R(copy)) = mcHWith(R(item), Info(R(ans)));
	}

	R(ans) = R(copy);
    }

    /* convert R(ans) and each element of R(ans) into sets */
    for (i = R(ans); i != Nil; i = Next(i)) {
	Info(i) = New_Set(Info(i));
    }
    R(ans) = New_Set(R(ans));
    LEAVE   R(ans);
}

Pd1(pdPow)
{
    ENTER;
    Reg     (ans);

    switch (Kind(x)) {
      case Set:
	R(ans) = powset(x);
	break;

      default:
        RT_ERROR("pow");
    }
    LEAVE   R(ans);
}

/* Return list of sublists of S of size n, in front of tail */
/* S is list */
static IPtr     npowlist(INT X PTR X PTR);
static IPtr npowlist(n, S, tail)
    int     n;
    IPtr     S,
            tail;
{
    ENTER;
    Reg     (ans);
    Reg     (item);
    Reg     (part);
    Reg     (combine);

    if (n == 0) {
	/* prepend the list containing the empty list */
	R(ans) = mcCons(Nil, tail);
    } else if (n > mcLen(S)) {
	/* prepend the empty list */
	R(ans) = tail;
    } else {
	R(item) = Info(S);
	/* prepend npow(n,Next(S)) */
	R(ans) = npowlist(n, Next(S), tail);
	/* add item to each element in npow(n-1,S) */
	R(part) = npowlist(n - 1, Next(S), Nil);
	while (R(part) != Nil) {
	    R(combine) = mcCons(R(item), Info(R(part)));
	    R(ans) = mcCons(R(combine), R(ans));
	    R(part) = Next(R(part));
	}
    }

    LEAVE   R(ans);
}

static IPtr     htree2list(PTR);
static IPtr htree2list(S)
    IPtr     S;
{
    ENTER;
    Reg     (ans);
    Reg     (list);
    Reg     (item);

    R(ans) = Nil;
    R(list) = mcStart(S);
    while (mcNext(&R(item), &R(list), true)) {
	R(ans) = mcCons(R(item), R(ans));
    }

    LEAVE   R(ans);
}

Pd2(pdNpow)
{
    ENTER;
    Reg     (ans);
    Reg     (list);
    Reg     (item);

    switch (Pair(Kind(a), Kind(b))) {
      case Bignum:
	NA("NPOW bignum");
	return OM;
      case Pair(Integer, Set):{
	    IPtr     t;

	    t = a;
	    a = b;
	    b = t;
	}			       /* fall thru */

      case Pair(Set, Integer):{
	    if (Int_Value(b) < 0)
		R(ans) = OM;
	    else {
		R(list) = htree2list(Set_Value(a));
		R(list) = npowlist(Int_Value(b), R(list), Nil);

		R(ans) = Nil;
		while (R(list) != Nil) {
		    R(item) = New_Set(Info(R(list)));
		    R(ans) = mcCons(R(item), R(ans));
		    R(list) = Next(R(list));
		}
		R(ans) = New_Set(R(ans));
	    }
	} break;

      default:
        RT_ERROR("npow");
    }

    LEAVE   R(ans);
}

Pd1(pdSign)
{

    switch (Kind(x)) {
      case Integer:{
	    C_Int   i;

	    i = Int_Value(x);
	    return New_Integer((WORKING) (i == 0 ? 0 : i > 0 ? 1 : -1));
	} break;

      case Bignum:
	return New_Integer((WORKING) (Length(x) == 0 ? 0 : Sign(x)));

      case Rational:
	return New_Integer((WORKING) Sign(Num(x)));

      case Real:{
	    C_Real  r;

	    r = Real_Value(x);
	    return New_Real(r == 0.0 ? 0.0 : r > 0.0 ? 1.0 : -1.0);
	} break;

      default:
        RT_ERROR("sign");
    }
    return OM;
}




FILE   *CONin = 0;
static IPtr     opener(PTR X STR);
static IPtr opener(x, mode)
    IPtr     x;
    char   *mode;
{
    C_File  f;

    switch (Kind(x)) {
      case String:
	if (strcmp(Str_Value(x), "CONSOLE") == 0) {
	    if (*mode == 'r') {
		f = stdin;
		CONin = f;
	    } else
		f = stdout;
	} else {
	    f = FOpen(Str_Value(x), mode);
	}
	if (f != FNULL)
	    return New_File(f, mode[0]);
	else
	    return OM;
      default:
	return OM;
    }
}

Pd1(pdEof)
{
    if (Kind(x) == File && File_Value(x) != FNULL) {
	return New_Boolean(File_Eof(x));
    } else {
	return OM;
    }
}

Pd1(pdOpenr)
{
    return opener(x, "r");
}

Pd1(pdOpenw)
{
    return opener(x, "w");
}

Pd1(pdOpena)
{
    return opener(x, "a");
}


Pd1(pdClose)
{
    FILE * f;
    switch (Kind(x)) {
    case File:
	f = File_Value(x);
	if ( f != FNULL  && f != stdin && f != stdout )
	    fclose(f);
	File_Value(x) = FNULL;
    }
    return NonPrint;
}




static IPtr     real_apply(PTR X MATHFUNC);
static IPtr     check_apply(PTR X PTRFUNC);

static IPtr real_apply(x, f)
    IPtr     x;
    MathReal(*f) (MATHREAL);
{
    C_Real  r;

    switch (Kind(x)) {
      case Integer:
      case Real:
      case Rational:
      case Bignum:
	r = Real_Value(mpFloat(x));
	break;
      default:
        RT_ERROR("predef needs number");
    }
    return New_Real((*f) (r));
}

static IPtr check_apply(x, f)
    IPtr     x;
    IPtr     (*f) (MATHREAL); 
{
    C_Real  r;

    switch (Kind(x)) {
      case Integer:
      case Real:
      case Rational:
      case Bignum:
	r = Real_Value(mpFloat(x));
	break;
      default:
	RT_ERROR("predef needs number");
    }
    return (*f) (r);
}

    IPtr     my_ln(MATHREAL);
    IPtr     my_log10(MATHREAL);
    IPtr     my_sqrt(MATHREAL);
    IPtr     my_acos(MATHREAL);
    IPtr     my_asin(MATHREAL);
    IPtr     my_sec(MATHREAL);
    IPtr     my_csc(MATHREAL);
    IPtr     my_cot(MATHREAL);
    IPtr     my_asec(MATHREAL);
    IPtr     my_acsc(MATHREAL);
    IPtr     my_acot(MATHREAL);
    IPtr     my_acosh(MATHREAL);
    IPtr     my_atanh(MATHREAL);

    MathReal Xsinh(r)
    MathReal r;
    {
	return ((exp(r) - exp(-r)) / (double) 2.0);
    }
    MathReal Xcosh(r)
    MathReal r;
    {
	return ((exp(r) + exp(-r)) / (double) 2.0);
    }
    MathReal Xtanh(r)
    MathReal r;
    {
	return ((exp(r) - exp(-r)) / (exp(r) + exp(-r)));
    }

    MathReal asinh(r)
    MathReal r;
    {
	return (log(r + sqrt(r * r + (double) 1.0)));
    }
    IPtr     my_acosh(r)
    MathReal r;
    {
	if (r < 1)
	    return OM;
	return New_Real(log(r + sqrt(r * r - 1.0)));
    }
    IPtr     my_atanh(r)
    MathReal r;
    {
	if (r <= -1 || r == 1)
	    return OM;
	return New_Real(log(((double) 1.0 + r)
			    / ((double) 1.0 - r))
			/ (double) 2.0);
    }

    Pd1(pdExp) {return  real_apply(x, exp);}
    Pd1(pdLn)  {return check_apply(x, my_ln);}
    Pd1(pdLog) {return check_apply(x, my_log10);}
    Pd1(pdSqrt){return check_apply(x, my_sqrt);}
    Pd1(pdTan) {return  real_apply(x, tan);}
    Pd1(pdCos) {return  real_apply(x, cos);}
    Pd1(pdSin) {return  real_apply(x, sin);}
    Pd1(pdATan){return  real_apply(x, atan);}
    Pd1(pdACos){return check_apply(x, my_acos);}
    Pd1(pdASin){return check_apply(x, my_asin);}
    Pd1(pdSec) {return check_apply(x, my_sec);}
    Pd1(pdCsc) {return check_apply(x, my_csc);}
    Pd1(pdCot) {return check_apply(x, my_cot);}
    Pd1(pdASec){return check_apply(x, my_asec);}
    Pd1(pdACsc){return check_apply(x, my_acsc);}
    Pd1(pdACot){return check_apply(x, my_acot);}
    Pd1(pdSinh){return  real_apply(x, Xsinh);}
    Pd1(pdCosh){return  real_apply(x, Xcosh);}
    Pd1(pdTanh){return  real_apply(x, Xtanh);}
    Pd1(pdAsinh){return real_apply(x, asinh);}
    Pd1(pdAcosh){return check_apply(x, my_acosh);}
    Pd1(pdAtanh){return check_apply(x, my_atanh);}

    IPtr     my_ln(r)
    C_Real  r;
    {
	if (r > 0)
	    return New_Real(log(r));
	else
	    return OM;
    }
    IPtr     my_log10(r)
    C_Real  r;
    {
	if (r > 0)
	    return New_Real(log10(r));
	else
	    return OM;
    }

    IPtr     my_sqrt(r)
    C_Real  r;
    {
	if (r >= 0)
	    return New_Real(sqrt(r));
	else
	    return OM;
    }

    IPtr     my_acos(r)
    C_Real  r;
    {
	if (-1 <= r && r <= 1)
	    return New_Real(acos(r));
	else
	    return OM;
    }

    IPtr     my_asin(r)
    C_Real  r;
    {
	if (-1 <= r && r <= 1)
	    return New_Real(asin(r));
	else
	    return OM;
    }


Pd1(pdLo){ 
    if( Kind(x) != Tuple ) {
	RT_ERROR("lo: needs tuple");
    }
    return New_Integer((WORKING)Tuple_Origin(x));
}

Pd1(pdHi){ 
    if( Kind(x) != Tuple ) {
	RT_ERROR("hi: needs tuple");
    }
    return New_Integer((WORKING)Tuple_Origin(x)+mcLen(x)-1);
}

Pd(pdOrigin){ 
    IPtr x = Info(args);
    Idx old = def_orig;
    if( Kind(x) == Integer ) {
	def_orig = Int_Value(x);
    }
    return New_Integer((WORKING)old);
}

Pd(pdSys){
    IPtr x = Info(args);
    char * cmd;

#ifdef Mac
    NA("system");
#else
    if( x == OM ) cmd = "";
    else if( Kind(x) == String ) cmd = Str_Value(x);
    else {
	RT_ERROR("system: needs string");
    }
#ifdef Screen
    sys(cmd);
#else
    system(cmd);
#endif
#endif
    return NonPrint;
}


Pd(pdAsString){
    IPtr x = Info(args);
    IPtr y = Info(Next(args));
    char text[80];
    int prec;
    switch(Kind(y)) {
        case Integer: prec = Int_Value(y); break;
        case Undefined: prec = r_prec; break;
        default: RT_ERROR("Bad precision to as_string");
    }

    switch( Kind(x) ) {
        case Integer:   sprintf(text,"%d", Int_Value(x)); break;

        case Bignum: case Rational:
	case Real:	sprintf(text,"%0.*g", prec, Real_Value(mpFloat(x)));
            break;
	case Closure: {
	    ENTER;
	    Reg(result);
	    IPtr code;
	    R(result) = New_String("");
	    for( code = Cl_SRC(x); code != Nil ; code = Next(code) ) {
		IPtr src  = Info(code);
		if( Kind(src) == String ) {
		    R(result) = mcStr_Concat( R(result), src );
		}
	    }
	    LEAVE R(result);
	} break;
	case Predef:
	    return New_String(Pd_name(x));
	default: {
	    return New_String("Not available as string");
	}
    }
    return New_String(text);
}

Pd1(pdNumerator){
    switch(Kind(x)) {
      case Rational: return Num(x);
      case Integer: case Bignum: return x;
      default: return OM;
    }
}

Pd1(pdDenominator){
    switch(Kind(x)) {
      case Rational: return Den(x);
      case Integer: case Bignum: return New_Integer((WORKING)1);
      default: return OM;
    }
}

IPtr my_sec(x) C_Real x;{
   C_Real c = cos(x); return c==0 ? OM : New_Real(1/c); }
IPtr my_csc(x) C_Real x;{
   C_Real c = sin(x); return c==0 ? OM : New_Real(1/c); }
IPtr my_cot(x) C_Real x;{
   C_Real c = tan(x); return c==0 ? OM : New_Real(1/c); }
IPtr my_asec(r) C_Real r;{
   return (-1 <= r && r <= 1) ?  OM : New_Real(acos(1/r)); }
IPtr my_acsc(r) C_Real r;{
   return (-1 <= r && r <= 1) ?  OM : New_Real(asin(1/r)); }
IPtr my_acot(r) C_Real r;{
   return New_Real(r==0 ?  acos(0) : atan(1/r)); }

