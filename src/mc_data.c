#include "common.h"
/* $Header: mc_data.c,v 3.2 90/04/25 16:23:07 gary Locked $ */
#include "micro.h"
#include "manager.h"
#include <ctype.h>



/*
 * (i,j,step): return list i..j by step
 */
IPtr     mcCnt(low, high, step)
    C_Int   low,
            high,
            step;
{
    ENTER;
    Reg     (ans);

    R(ans) = Nil;

    if (step != 0) {
	for (; (step > 0 ? low <= high : low >= high); low += step) {
	    R(ans) = mcCons(Nil, R(ans));
	    Info(R(ans)) = New_Integer((WORKING) low);
	}
    }
    R(ans) = mcRev(R(ans));
    LEAVE   R(ans);
}



/*
 * (i,j,step): return list i..j by step
 */
IPtr     RatCnt(low, high, step)
    IPtr    low,
            high,
            step;
{
    ENTER;
    Reg(i);
    Reg     (ans);

    R(ans) = Nil;
    R(i)   = low;

    if (Length(Num(step))!=0) {
	for (; (Sign(Num(step)) > 0 ?  !mcLt(high,R(i)) : !mcLt(R(i),high));
              R(i) = RatAdd(R(i),step)) {
	    R(ans) = mcCons(Nil, R(ans));
	    Info(R(ans)) = R(i);
	}
    }
    R(ans) = mcRev(R(ans));
    LEAVE   R(ans);
}



/* (a,b:String):String Concat a and b */
IPtr     mcStr_Concat(a, b)
    IPtr     a,
            b;
{
    C_Str   Ca = Str_Value(a);
    C_Str   Cb = Str_Value(b);
    C_Str   Cp;

    ENTER;
    Reg     (ans);

    assert(Kind(a) == String && Kind(b) == String);

    R(ans) = New_Block(String, (strlen(Ca) + strlen(Cb) + 1));
    Cp = Str_Value(R(ans));

    strcpy(Cp, Ca);
    strcat(Cp, Cb);
    LEAVE   R(ans);
}



/* (n:int,L:Tree):Tree n repetitions of L */
IPtr     mcRep(n, L)
    C_Int   n;
    IPtr     L;
{
    ENTER;
    Reg     (ans);

    R(ans) = NilT;
    while (n > 0) {
	R(ans) = mcCat(L, R(ans));
	n--;
    }

    LEAVE   R(ans);
}

/* (n:int,S:string):string n repetitions of S */
IPtr     mcStr_Rep(n, S)
    C_Int   n;
    IPtr     S;
{
    ENTER;
    Reg     (ans);

    assert(Kind(S) == String);

    R(ans) = New_String("");
    while (n > 0) {
	R(ans) = mcStr_Concat(S, R(ans));
	n--;
    }

    LEAVE   R(ans);
}



/* (x,y:C_Str):bool x substr of y */
Bool    mcSubstr(x, y)
    C_Str   x,
            y;
{
    int     x_len = strlen(x);
    int     y_len = strlen(y);
    int     i;

    for (i = 0; i <= y_len - x_len; i++) {
	if (strncmp(x, y + i, x_len) == 0)
	    return true;
    }
    return false;
}



/* (x):int Length of x */
Idx     mcLen(x)
    IPtr     x;
{
    switch (Kind(x)) {
      case List:
	{
	    Idx     count = 0;

	    while (x != Nil) {
		count++;
		x = Next(x);
	    }
	    return count;
	}

      case Tree:
	return Len(x);
      case HTree:
	return HLen(x);
      case Tuple:
	return mcTLen(Tuple_Value(x));
      case Set:
	return mcHTLen(Set_Value(x));
      case String:
	return strlen(Str_Value(x));

      default:
	BAD_ARGS("#", NULL, x);
    }
    return 0;			       /* never taken, for Lint */
}



/* (x,L):List cons(x,L) */
IPtr     mcCons(x, L)
    IPtr     x,
            L;
{
    ENTER;
    Reg     (ans);

    assert(x && Kind(L) == List);

    R(ans) = New_Block(List, 0);
    Info(R(ans)) = x;
    Next(R(ans)) = L;

    LEAVE   R(ans);
}



/* (x,L):bool x in? L */
Bool    mcIn_List(x, L)
    IPtr     x,
            L;
{
    assert(x && Kind(L) == List);

    while (L != Nil) {
	if (mcEq(x, Info(L)))
	    return true;
	L = Next(L);
    }
    return false;
}



/* (x): Return x in reverse order */
IPtr     mcRev(x)
    IPtr     x;
{
    ENTER;
    Reg     (ans);

    assert(Kind(x) == List);

    R(ans) = Nil;

    /* copy list in reverse order */
    while (x != Nil) {
	R(ans) = mcCons(Info(x), R(ans));
	x = Next(x);
    }

    LEAVE   R(ans);
}




/* (x,y):bool x==y */
Bool    mcEq(x, y)
    IPtr     x,
            y;
{
    if (x == y)
	return true;
    if (Kind(x) == Kind(y)) {

	switch (Kind(x)) {
	  case File:
	  case Predef:
	  case Closure:
	    return x == y;

	  case Atom:
	    return Atom_Value(x) == Atom_Value(y);

	  case Boolean:
	    return Bool_Value(x) == Bool_Value(y);

	  case Integer:
	    return Int_Value(x) == Int_Value(y);

	  case Bignum:
	    return mpCmp(x, y) == 0;

	  case List:
	  case Tree:
	    {			       /* answer remains invariant under
				        * loop */
		ENTER;
		Reg     (x_value);
		Reg     (y_value);
		Reg     (x_list);
		Reg     (y_list);
		Bool    got_x,
		        got_y;

		R(x_list) = mcStart(x);
		R(y_list) = mcStart(y);

		got_x = mcNext(&R(x_value), &R(x_list), true);
		got_y = mcNext(&R(y_value), &R(y_list), true);

		while (got_x && got_y && mcEq(R(x_value), R(y_value))) {
		    got_x = mcNext(&R(x_value), &R(x_list), true);
		    got_y = mcNext(&R(y_value), &R(y_list), true);
		}
		LEAVE ! got_x && !got_y;
	    }

	  case Real:
	    return Real_Value(x) == Real_Value(y);

	  case Rational:
	    return mcEq(Num(x),Num(y)) && mcEq(Den(x),Den(y));

	  case HTree:
	    return mcLen(x) == mcLen(y)
		&& mcSubset(x, y)
		&& mcSubset(y, x);

	  case Set:
	    return mcLen(x) == mcLen(y)
		&& mcHash(x,false) == mcHash(y,false)
	        && mcSubset(Set_Value(x), Set_Value(y))
		&& mcSubset(Set_Value(y), Set_Value(x));

	  case String:
	    return 0 == strcmp(Str_Value(x), Str_Value(y));

	  case Tuple:
	    return mcLen(x) == mcLen(y)
		&& mcHash(x,false) == mcHash(y,false)
		&& Tuple_Origin(x) == Tuple_Origin(y)
		&& mcEq(Tuple_Value(x), Tuple_Value(y));
		     

	  case Undefined:
	    return true;

	  default:
	    mcprintf("x=%p y=%p", x,y);
	    mcprintf("Kind(x)=%d\n", Kind(x));
	    assert(0);
	}
    } else {
	ENTER;
	Reg     (temp);
	Bool    result;

	switch (Pair(Kind(x), Kind(y))) {
	  case Pair(Integer, Real):
	    LEAVE(C_Real) Int_Value(x) == Real_Value(y);
	  case Pair(Real, Integer):
	    LEAVE Real_Value(x) == (C_Real) Int_Value(y);
	  case Pair(Bignum, Integer):
	    R(temp) = New_Big((WORKING) Int_Value(y));
	    result = mpCmp(x, R(temp)) == 0;
	    LEAVE   result;

	  case Pair(Rational, Real):
	      R(temp) = mpFloat(x);
	      LEAVE Real_Value(R(temp)) == Real_Value(y);

	  case Pair(Real, Rational):
	      R(temp) = mpFloat(y);
	      LEAVE Real_Value(x) == Real_Value(R(temp));

	  case Pair(Integer, Bignum):
	    R(temp) = New_Big((WORKING) Int_Value(x));
	    result = mpCmp(R(temp), y) == 0;
	    LEAVE   result;

	  case Pair(Real, Bignum):
	    R(temp) = mpFix(Real_Value(x));
	    result =   mcEq(R(temp), y);
	    LEAVE result;


	  case Pair(Bignum, Real):
	    R(temp) = mpFix(Real_Value(y));
	    result =   mcEq(x, R(temp));
	    LEAVE result;


	  default:
	    LEAVE false;
	}
    }
}



/* (x,y):bool x<y */
Bool    mcLt(x, y)
    IPtr     x,
            y;
{
    if (Kind(x) == Kind(y)) {
	switch (Kind(x)) {
	  case Integer:
	    return Int_Value(x) < Int_Value(y);

	  case Bignum:
	    return mpCmp(x, y) < 0;

	  case Real:
	    return Real_Value(x) < Real_Value(y);

	  case Rational: {
	      ENTER;
	      Reg(left);
	      Reg(right);
	      Bool result;
	      R(left) = mpTimes(Num(x),Den(y));
	      R(right)= mpTimes(Num(y),Den(x));
	      result =  mcLt(R(left),R(right));
	      LEAVE result;
	  }

	  case String:
	    return strcmp(Str_Value(x), Str_Value(y)) < 0;

	  default:
	    BAD_ARGS("<relation>", x, y);
	}
    } else {
	ENTER;
	Reg     (tx);
	Reg     (ty);
	Bool    result;

	/* mixed operations */
	switch (Pair(Kind(x), Kind(y))) {
	  case Pair(Rational,Integer):
	  case Pair(Rational,Bignum):
	  case Pair(Integer,Rational):
	  case Pair(Bignum,Rational):
	      R(tx) = RatForce(x);
	      R(ty) = RatForce(y);
	      result = mcLt(R(tx),R(ty));
	      LEAVE result;

	  case Pair(Integer, Real):
	    LEAVE(C_Real) Int_Value(x) < Real_Value(y);
	    break;

	  case Pair(Real, Integer):
	    LEAVE Real_Value(x) < (C_Real) Int_Value(y);
	    break;
	  case Pair(Bignum, Integer):
	    R(ty) = New_Big((WORKING) Int_Value(y));
	    result = mpCmp(x, R(ty)) < 0;
	    LEAVE   result;

	  case Pair(Integer, Bignum):
	    R(tx) = New_Big((WORKING) Int_Value(x));
	    result = mpCmp(R(tx), y) < 0;
	    LEAVE   result;

	  case Pair(Real,Rational):
	  case Pair(Real, Bignum):
	  case Pair(Rational,Real):
	  case Pair(Bignum, Real):
	    R(tx) = mpFloat(x);
	    R(ty) = mpFloat(y);
	    LEAVE   Real_Value(R(tx)) < Real_Value(R(ty));

	  default:
	    BAD_ARGS("<relation>", x, y);
	}
    }
}



/* (lhs,rhs)  lhs:= rhs */
/*
 * acceptable possibilities are: ref := rhs	list := om	tuple :=
 * tuple	tuple := list
 */
void    mcAssign(plhs, prhs)
    IPtr     plhs,
            prhs;
{
    ENTER;
    Reg     (lhs);
    Reg     (rhs);
    Reg     (i_lhs);
    Reg     (i_rhs);

    R(lhs) = plhs;
    R(rhs) = prhs;

    switch (Kind(R(lhs))) {
      case Ref:

	if (Watched(Var_Index(Var_Assoc(plhs)))) {
	    show("! ", stdout);
	    dbPrint(plhs, stdout);
	    show(" := ", stdout);
	    dbPrint(prhs, stdout);
	    mcFlush(stdout);
	}
	Var(R(lhs)) = mcModify(Var(R(lhs)),
			       Sel_List(R(lhs)),
			       R(rhs));
	break;

      case Tuple:
	/* R(rhs) := list of R(rhs) values */
	switch (Kind(R(rhs))) {
	  case Undefined:
	    R(rhs) = Nil;
	    break;
	  case Tuple:
	    R(rhs) = mcStart(Tuple_Value(R(rhs)));
	    break;
	  case List:
	    R(rhs) = mcStart(R(rhs));
	    break;
	  default:
	    BAD_ARGS(":=", R(lhs), R(rhs));
	}
	R(lhs) = mcStart(Tuple_Value(R(lhs)));
	/* assign corresponding elements */
	while (mcNext(&R(i_lhs), &R(lhs), true)) {
	    mcNext(&R(i_rhs), &R(rhs), true);
	    mcAssign(R(i_lhs), R(i_rhs));
	}
	break;

      case Undefined:
	/* Dispose of result */
	break;

      default:
	BAD_ARGS(":=", R(lhs), R(rhs));
    }
    LEAVE;
}


