/* $Header: mc_tuple.c,v 3.1 90/04/25 16:23:26 gary Locked $ */
#include "common.h"
#include "micro.h"
#include "manager.h"

/*  static forward declarations */
static IPtr     mcLTrim(PTR);
static IPtr     mcTTrim(PTR);

/* (x:List):List Return x with trailing OMs deleted (destructive) */
static IPtr mcLTrim(x)
    IPtr     x;
{
    IPtr     non_om,      /* ptr to last non-om value before p in x */
            p;

    p = x;
    non_om = OM;
    while (p != Nil) {
	if (Info(p) != OM)
	    non_om = p;
	p = Next(p);
    }

    if (non_om != OM)
	Next(non_om) = Nil;
    else
	x = Nil;

    return x;
}



/* (x:Tree):Tree Return x with trailing OMs deleted */
static IPtr mcTTrim(x)
    IPtr     x;
{
    ENTER;
    Reg     (ans);

    if (x == NilT || x == OM)  /* empty */
	R(ans) = NilT;
    else if (Kind(x) != Tree)  /* singleton */
	R(ans) = x;
    else {
	R(ans) = mcTTrim(Right(x)); 
	if (R(ans) == Right(x))     /* unchanged */
	    R(ans) = x;
	else if (R(ans) == NilT)  /* empty right */
	    R(ans) = mcTTrim(Left(x));
	else          /* keep left plus trimmed right */
	    R(ans) = mcCat(Left(x), R(ans));
    }
    LEAVE   R(ans);
}


/* (x:Tuple-rep):Tuple-rep returns x with trailing om's deleted 
 * Destroys lists.
 */
IPtr     mcTrim(x)
    IPtr     x;
{
    if (Kind(x) == List)
	return mcLTrim(x);
    else
	return mcTTrim(x);
}





/* (lhs,rhs) take lhs fromb rhs */
void    mcFromb(lhs, rhs)
    IPtr     lhs,
            rhs;
{
    ENTER;
    Reg     (rval);
    Reg     (tmp);
    Idx     len;

    R(rval) = mcVal(rhs);

    switch (Kind(R(rval))) {
      case Tuple:{
	    R(tmp) = mcAccess(Tuple_Value(R(rval)), (Idx) 0);
	    mcAssign(lhs, R(tmp));

	    R(tmp) = mcTSlice(Tuple_Value(R(rval)),
			      (Idx) 1, mcLen(R(rval))-1);
	    R(tmp) = New_Tuple(R(tmp), Tuple_Origin(R(rval)));
	    mcAssign(rhs, R(tmp));
	} break;

      case String:{
	    len = mcLen(R(rval));
	    if (len == 0) {
		mcAssign(lhs, OM);
	    } else {
		R(tmp) = mcSSlice(R(rval), (Idx) 1, (Idx) 1);
		mcAssign(lhs, R(tmp));
		R(tmp) = mcSSlice(R(rval), (Idx) 2, len);
		mcAssign(rhs, R(tmp));
	    }
	} break;
      default:
	BAD_ARGS("fromb", lhs, rhs);
    }

    LEAVE;
}

/* (lhs,rhs) take lhs frome rhs */
void    mcFrome(lhs, rhs)
    IPtr     lhs,
            rhs;
{
    ENTER;
    Reg     (rval);
    Reg     (tmp);
    Idx     len;

    R(rval) = mcVal(rhs);

    switch (Kind(R(rval))) {
      case Tuple:{
	    len = mcLen(R(rval));
	    if (len == 0) {
		mcAssign(lhs, OM);
	    } else {
		R(tmp) = mcAccess(Tuple_Value(R(rval)), len-1);
		mcAssign(lhs, R(tmp));

		R(tmp) = mcTSlice(Tuple_Value(R(rval)), (Idx) 0, len - 2);
		R(tmp) = New_Tuple(R(tmp), Tuple_Origin(R(rval)));
		mcAssign(rhs, R(tmp));
	    }
	} break;

      case String:{
	    len = mcLen(R(rval));
	    if (len == 0) {
		mcAssign(lhs, OM);
	    } else {
		R(tmp) = mcSSlice(R(rval), len, len);
		mcAssign(lhs, R(tmp));
		R(tmp) = mcSSlice(R(rval), (Idx) 1, len - 1);
		mcAssign(rhs, R(tmp));
	    }
	} break;

      default:
	BAD_ARGS("frome", lhs, rhs);
    }
    LEAVE;
}



/* (x,y:IPtr):Tuple [x,y] */

IPtr     mcTwo_Tuple(x, y)
    IPtr     x,
            y;
{
    ENTER;
    Reg     (ans);

    R(ans) = mcCons(y, Nil);
    R(ans) = mcCons(x, R(ans));
    R(ans) = New_Tuple(R(ans),def_orig);
    LEAVE   R(ans);
}
