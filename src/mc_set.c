/* $Header: mc_set.c,v 3.1 90/04/25 16:23:16 gary Locked $ */
#include "common.h"
#include "micro.h"
#include "manager.h"


/*  static forward declarations */
static IPtr     LDel_Image(PTR X PTR);
static IPtr     HDel_Image(PTR X PTR X INT);


/* (f:list, i):list   f-{[i,.]} */
static IPtr LDel_Image(f, i)
    IPtr     f,
            i;
{
    ENTER;
    Reg     (ans);
    Reg     (list);
    Reg     (item);

    R(ans) = NilHT;
    for (R(list) = mcStart(f); mcNext(&R(item), &R(list), true);) {
	IPtr     pair = R(item);

	if (Kind(pair) == Tuple
	    && mcLen(pair) == 2
	    && !mcEq(i, mcAccess(Tuple_Value(pair), (Idx) 0))
	    ) {
	    R(ans) = mcHWith(pair, R(ans));
	}
    }
    LEAVE   R(ans);
}

static IPtr HDel_Image(f, i, h)
    IPtr     f,
            i;
    int     h;
{
    ENTER;
    Reg     (ans);

    if (f == NilHT)
	R(ans) = f;
    else if (Kind(f) != HTree) {
	if (mcHash(f, true) == h)
	    R(ans) = LDel_Image(f, i);
	else
	    R(ans) = f;
    } else if (h <= mcHMax(HLeft(f))) {
	R(ans) = HDel_Image(HLeft(f), i, h);
	R(ans) = mcHCat(R(ans), HRight(f));
    } else {
	R(ans) = HDel_Image(HRight(f), i, h);
	R(ans) = mcHCat(HLeft(f), R(ans));
    }
    LEAVE   R(ans);
}

IPtr     mcDel_Image(f, i)
    IPtr     f,
            i;
{
    return HDel_Image(f, i, mcHash(i, false));
    /* i is used as an index, not a set element */
}


/* (f:htree, i):htree	{y: y=f(x) | x=i}  */
IPtr     mcImages(f, i)
    IPtr     f,
            i;
{
    ENTER;
    Reg     (item);
    Reg     (list);
    Reg     (ans);
    int     h = mcHash(i, false);

    if (f == NilHT) {
	R(ans) = NilHT;
    } else {
	/* find right leaf (list or singleton) */
	while (Kind(f) == HTree) {
	    if (h <= mcHMax(HLeft(f)))
		f = HLeft(f);
	    else
		f = HRight(f);
	}

	R(ans) = NilHT;
	if (h == mcHash(f, true)) {    /* extract from list */
	    R(list) = mcStart(f);
	    while (mcNext(&R(item), &R(list), true)) {
		IPtr     pair = R(item);

		if (Kind(pair) == Tuple
		    && mcLen(pair) == 2
		    && mcEq(i, mcAccess(Tuple_Value(pair), (Idx) 0))
		    ) {
		    R(ans) = mcHWith(mcAccess(Tuple_Value(pair), (Idx) 1),
				     R(ans));
		}
	    }
	}
    }
    LEAVE   R(ans);
}






/* (i:*, L:list):list	L-{i} */
IPtr     mcLLess(i, L)
    IPtr     i,
            L;
{
    ENTER;
    Reg     (ans);

    if (!mcIn_List(i, L)) {
	R(ans) = L;
    } else {
	R(ans) = Nil;
	for (; L != Nil; L = Next(L)) {
	    IPtr     elem = Info(L);

	    if (!mcEq(i, elem)) {
		R(ans) = mcCons(elem, R(ans));
	    }
	}
    }

    LEAVE   R(ans);
}




/* (lhs,rhs) take lhs from rhs */
void    mcFrom(lhs, rhs)
    IPtr     lhs,
            rhs;
{
    ENTER;
    Reg     (rval);
    Reg     (tmp);

    R(rval) = mcVal(rhs);

    switch (Kind(R(rval))) {
      case Set:{
	    if (Set_Value(R(rval)) == NilHT) {
		mcAssign(lhs, OM);
	    } else {
		R(tmp) = mcHAny(Set_Value(R(rval)));
		mcAssign(lhs, R(tmp));
		R(tmp) = mcLess(R(tmp), R(rval));
		mcAssign(rhs, R(tmp));
	    }
	} break;

      case Tuple:{
	    R(tmp) = mcAccess(Tuple_Value(R(rval)), (Idx) 0);
	    mcAssign(lhs, R(tmp));

	    R(tmp) = mcTSlice(Tuple_Value(R(rval)),
			      (Idx) 1, mcLen(R(rval))-1);
	    R(tmp) = New_Tuple(R(tmp), Tuple_Origin(R(rval)));
	    mcAssign(rhs, R(tmp));
	} break;

      default: 
	BAD_ARGS("from", lhs, rhs);
    }

    LEAVE;
}



/* (i:*, S:set):set   S+{i} */
IPtr     mcWith(i, S)
    IPtr     i,
            S;
{
    ENTER;
    Reg     (ans);

    if (Kind(i) == Undefined) {
	R(ans) = OM;
    } else if (mcIn_HTree(i, Set_Value(S))) {
	R(ans) = S;
    } else {
	R(ans) = New_Block(Set, 0);
	Set_Value(R(ans)) = mcHWith(i, Set_Value(S));
    }

    LEAVE   R(ans);
}



/* (i:*, S:set):set   S-{i} */
IPtr     mcLess(i, S)
    IPtr     i,
            S;
{
    ENTER;
    Reg     (ans);

    if (Kind(i) == Undefined) {
	R(ans) = OM;
    } else if (!mcIn_HTree(i, Set_Value(S))) {
	R(ans) = S;
    } else {
	R(ans) = New_Block(Set, 0);
	Set_Value(R(ans)) = mcHLess(i, Set_Value(S));
    }

    LEAVE   R(ans);
}
