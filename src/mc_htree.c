#include "common.h"
/* $Header: mc_htree.c,v 3.2 90/04/25 16:23:10 gary Locked $ */
#include "micro.h"
#include "manager.h"



/* AVL trees ordered by hash values */


/* elements with equal hash values in lists
 *
 * An HTree is either
 *	NilHT,
 *	any real ISETL type for singleton (not List or HTree),
 *	a non-empty List,
 *	or an HTree node
 *
 * There are two kinds of hash values, those used to look up elements
 * and those used to compute hash values of composite elements.  Elements
 * in a list have equal "element" hash values, but not necessarily
 * equal "full" hash values.  See mcHash below.

 */

/* static forward declaration */
void    mcNSBal(PTR ADDR);
IPtr     Add_To_Bin(PTR X PTR);
IPtr     Take_From_Bin(PTR X PTR);
IPtr     HCat(PTR X PTR);


#define Singleton(x)		x

/* Add x to L, where everything in L has same hash as x */
static IPtr Add_To_Bin(x, L)
    IPtr     x,
            L;
{
    ENTER;
    Reg     (ans);
    if (Kind(L) != List) {	       /* singleton bin */
	if (mcEq(x, L)) {
	    R(ans) = L;
	} else {
	    R(ans) = mcCons(L, Nil);
	    R(ans) = mcCons(x, R(ans));
	}
    } else {
	if (mcIn_List(x, L)) {
	    R(ans) = L;
	} else {
	    R(ans) = mcCons(x, L);
	}
    }
    LEAVE   R(ans);
}


/* Remove x from L, where everything in L has same hash as x */
static IPtr Take_From_Bin(x, L)
    IPtr     x,
            L;
{
    ENTER;
    Reg     (ans);

    if (Kind(L) != List) {	       /* singleton bin */
	if (mcEq(x, L))
	    R(ans) = Nil;
	else
	    R(ans) = L;
    } else {
	R(ans) = mcLLess(x, L);
    }
    LEAVE   R(ans);
}


/* Return hash value for x.  Some elements has differently depending
 * on whether they are elements of the set or sub-parts of an element.
 * Ex: X=[[2,3],4] hashes on the value of [2,3], because [2,3] can be
 * used to look up X (SMap).  [2,3] *in this context* hashes based on all
 * its parts, because it is not an element of the set.
 */
int     mcHash(x, is_element)
    IPtr     x;
    Bool    is_element;
{
    WORKING h;
    switch (Kind(x)) {
      case Atom:
	return Atom_Value(x);

      case Boolean:
	return (x == ITRUE ? 18 : 21);

      case Integer:
	return Int_Value(x);

      case Bignum:{
	    int     i;

	    if (mpToInt(x, &h))
		return h;

	    h = 0;
	    for (i = 0; i < Length(x); i++) {
		h = h * i + Digits(x)[i];
	    }
	    return h;
	}

      case Rational: {
          ENTER;
	  Reg(temp);
	  R(temp) = mpFloat(x);
	  h = mcHash(R(temp),is_element);
	  LEAVE h;
      }
	/* Integral values hash to same as integer */
      case Real:{
	    C_Real  r = Real_Value(x);
	    WORKING i;

	    if (r < 0)
		r = -r;
	    if (r < BASE) {
		i = r;
		return i + 10000 * (r - i);
	    } else {
		ENTER;
		Reg     (temp);

		R(temp) = mpFix(r);
		h =    mcHash(R(temp), is_element);
		LEAVE h;
	    }
	}

      case List:
	return (x == Nil ? 1 : mcHash(Info(x), is_element));

      case Tuple:{
	    ENTER;
	    int     i;

	    if (is_element && mcTLen(Tuple_Value(x)) == 2) {
		h = mcHash(mcAccess(Tuple_Value(x), (Idx) 0), false);
	    } else {
		if ((h = Tuple_Hash(x)) == 0) {
		    Reg     (x_list);
		    Reg     (item);

		    R(x_list) = mcStart(Tuple_Value(x));
		    i = h = 17;
		    while (mcNext(&R(item), &R(x_list), true)) {
			i++;
			h = h * i + mcHash(R(item), false);
		    }
		    Tuple_Hash(x) = h;
		}
	    }

	    LEAVE   h;
	}

      case Set:{
	    ENTER;
	    int     t;

	    if ((h = Set_Hash(x)) == 0) {
		Reg     (x_list);
		Reg     (item);

		/* Compute hash based on full hash values, in
		 * an order independent fashion */
		R(x_list) = mcStart(Set_Value(x));
		h = 0;
		while (mcNext(&R(item), &R(x_list), true)) {
		    t = mcHash(R(item), false);
		    h += (t+13)*(t+17);
		}
		Set_Hash(x) = h;
	    }
	    LEAVE   h;
	}

      case String:{
	    int     i;
	    int     len = mcLen(x);
	    h = len;

	    for (i = 0; i < len; i++) {
		h = h * i + Str_Value(x)[i];
	    }
	    return h;
	}

      default:
	return 17;
    }
}




/* assumes neither is empty */
static IPtr HCat(a, b)
    IPtr     a,
            b;
{
    ENTER;
    Reg     (t1);
    Reg     (t2);
    Reg     (ans);
    int     delta = mcHHgt(a) - mcHHgt(b);

    if (-max_delta <= delta && delta <= max_delta) {
	R(ans) = New_HTree(a, b);

    } else if (delta > 0) {	       /* a is taller */
	int     subdelta = mcHHgt(HLeft(a)) - mcHHgt(HRight(a));

	if (subdelta >= 0) {
	    R(t1) = HCat(HRight(a), b);
	    R(ans) = New_HTree(HLeft(a), R(t1));

	} else {
	    R(t1) = New_HTree(HLeft(a), HLeft(HRight(a)));
	    R(t2) = HCat(HRight(HRight(a)), b);
	    R(ans) = HCat(R(t1), R(t2));
	}
    } else {			       /* b is taller */
	int     subdelta = mcHHgt(HLeft(b)) - mcHHgt(HRight(b));

	if (subdelta <= 0) {
	    R(t1) = HCat(a, HLeft(b));
	    R(ans) = New_HTree(R(t1), HRight(b));

	} else {
	    R(t1) = HCat(a, HLeft(HLeft(b)));
	    R(t2) = New_HTree(HRight(HLeft(b)), HRight(b));
	    R(ans) = HCat(R(t1), R(t2));
	}
    }

    LEAVE   R(ans);
}

/* Catenate a and b */
IPtr     mcHCat(a, b)
    IPtr     a,
            b;
{

    if (a == NilHT)
	return b;
    else if (b == NilHT)
	return a;
    else
	return HCat(a, b);
}




/* (x:*,T:HTree):bool x in? T */
Bool    mcIn_HTree(x, T)
    IPtr     x,
            T;
{
    int     h = mcHash(x, true);

    if (T == NilHT)
	return false;
    if (h > mcHMax(T))
	return false;
    while (Kind(T) == HTree) {
	if (h <= mcHMax(HLeft(T)))
	    T = HLeft(T);
	else
	    T = HRight(T);
    }
    return (Kind(T) == List ? mcIn_List(x, T) : mcEq(x, T));
}






/* (x:element, T:HTree): HTree  T with x */
IPtr     mcHWith(x, T)
    IPtr     x,
            T;
{
    ENTER;
    Reg     (ans);
    int     h = mcHash(x, true);

    if (Kind(x) == Undefined || Kind(T) == Undefined) {
	R(ans) = OM;
    } else if (T == NilHT) {
	R(ans) = Singleton(x);
    } else if (Kind(T) != HTree) {
	int     T_hash = mcHash(T, true);

	if (h == T_hash) {
	    R(ans) = Add_To_Bin(x, T);
	} else if (h < T_hash) {
	    R(ans) = Singleton(x);
	    R(ans) = mcHCat(R(ans), T);
	} else {
	    R(ans) = Singleton(x);
	    R(ans) = mcHCat(T, R(ans));
	}
    } else if (h <= mcHMax(HLeft(T))) {
	R(ans) = mcHWith(x, HLeft(T));
	R(ans) = mcHCat(R(ans), HRight(T));
    } else {
	R(ans) = mcHWith(x, HRight(T));
	R(ans) = mcHCat(HLeft(T), R(ans));
    }

    LEAVE   R(ans);
}

/* (x:element, T:HTree): HTree  T less x */
IPtr     mcHLess(x, T)
    IPtr     x,
            T;
{
    ENTER;
    Reg     (ans);
    int     h = mcHash(x, true);

    if (T == NilHT) {
	R(ans) = T;
    } else if (Kind(T) != HTree) {
	int     T_hash = mcHash(T, true);

	if (h == T_hash) {
	    R(ans) = Take_From_Bin(x, T);
	    if (R(ans) == Nil)
		R(ans) = NilHT;
	} else {
	    R(ans) = T;
	}
    } else if (h <= mcHMax(HLeft(T))) {
	R(ans) = mcHLess(x, HLeft(T));
	R(ans) = mcHCat(R(ans), HRight(T));
    } else {
	R(ans) = mcHLess(x, HRight(T));
	R(ans) = mcHCat(HLeft(T), R(ans));
    }

    LEAVE   R(ans);
}




/* (x,y:htree):bool x subset of y */
Bool    mcSubset(x, y)
    IPtr     x,
            y;
{
    ENTER;
    Reg     (item);
    Reg     (x_list);

    R(x_list) = mcStart(x);
    while (mcNext(&R(item), &R(x_list), true)) {
	if (!mcIn_HTree(R(item), y)) {
	    LEAVE   false;
	}
    }
    LEAVE   true;
}


/* (x,y:HTree ):HTree Elem in both x and y */
IPtr     mcInter(x, y)
    IPtr     x,
            y;
{
    ENTER;
    Reg     (ans);
    Reg     (x_list);
    Reg     (item);

    if (mcHTLen(x) > mcHTLen(y)) {
	IPtr     t;

	t = x;
	x = y;
	y = t;
    }
    R(ans) = NilHT;
    R(x_list) = mcStart(x);
    while (mcNext(&R(item), &R(x_list), true)) {
	if (mcIn_HTree(R(item), y)) {
	    R(ans) = mcHWith(R(item), R(ans));
	}
    }

    LEAVE   R(ans);
}



/* (x,y:HTree):HTree Elem in x or y */
IPtr     mcUnion(x, y)
    IPtr     x,
            y;
{
    ENTER;
    Reg     (ans);
    Reg     (x_list);
    Reg     (item);

    if (mcHTLen(x) > mcHTLen(y)) {
	IPtr     t;

	t = x;
	x = y;
	y = t;
    }
    R(ans) = y;
    R(x_list) = mcStart(x);
    while (mcNext(&R(item), &R(x_list), true)) {
	if (!mcIn_HTree(R(item), y)) {
	    R(ans) = mcHWith(R(item), R(ans));
	}
    }

    LEAVE   R(ans);
}



/* (x,y:HTree):HTree Elem in x and not y */
IPtr     mcDiff(x, y)
    IPtr     x,
            y;
{
    ENTER;
    Reg     (ans);
    Reg     (y_list);
    Reg     (item);

    R(ans) = x;
    R(y_list) = mcStart(y);
    while (mcNext(&R(item), &R(y_list), true)) {
	if (mcIn_HTree(R(item), x)) {
	    R(ans) = mcHLess(R(item), R(ans));
	}
    }

    LEAVE   R(ans);
}


/* (S:HTree): Element  arbitrary element of S */
IPtr     mcHAny(S)
    IPtr     S;
{
    Idx     i;

    if (S == NilHT)
	return OM;

    i = rrand() * mcHTLen(S);	       /* Choose index of element */
    while (Kind(S) == HTree) {
	if (i < mcHTLen(HLeft(S)))
	    S = HLeft(S);	       /* index in Left */
	else {			       /* index in Right */
	    i = i - mcHTLen(HLeft(S)); /* adjust offset */
	    S = HRight(S);
	}
    }

    if (Kind(S) == List) {
	while (i > 0) {
	    S = Next(S);
	    i--;
	}
	return Info(S);
    } else
	return S;		       /* Singleton */
}




/*
 * These are procedures to MODIFY HTrees. Only use when you are certain
 * there is **No Sharing**
 */

/* assumes non-empty HTree with balanced subtrees.
 * *PT and everything beneath it are active
 */
static void mcNSBal(PT)
    IPtr    *PT;
{
    IPtr     left,
            right;
    int     delta;

    left = HLeft(*PT);
    right = HRight(*PT);
    delta = mcHHgt(left) - mcHHgt(right);

    if (-max_delta <= delta && delta <= max_delta) {
	/* do nothing, balanced */

    } else if (delta > 0) {	       /* left is taller */
	IPtr     LL = HLeft(left);
	IPtr     LR = HRight(left);
	int     subdelta = mcHHgt(LL) - mcHHgt(LR);

	if (subdelta >= 0) {
	    /*-
	     * (LL left LR) PT right
	     *      ==> LL left (LR PT right)
	     */
	    HLeft(*PT) = LR;
	    HRight(left) = *PT;
	    *PT = left;
	    HTFix(HRight(*PT));
	} else {
	    IPtr     LRL = HLeft(LR);
	    IPtr     LRR = HRight(LR);

	    /*-
	     * (LL left [LRL LR LRR]) PT right
	     *   ==> (LL left LRL) LR (LRR PT right)
	     */
	    HRight(left) = LRL;
	    HLeft(*PT) = LRR;
	    HLeft(LR) = left;
	    HRight(LR) = *PT;
	    *PT = LR;
	    HTFix(HLeft(*PT));
	    HTFix(HRight(*PT));
	}
    } else {			       /* right is taller */
	IPtr     RL = HLeft(right);
	IPtr     RR = HRight(right);
	int     subdelta = mcHHgt(RL) - mcHHgt(RR);

	if (subdelta <= 0) {
	    /*-
	     * left PT (RL right RR)
	     *  ==> (left PT RL) right RR
	     */
	    HRight(*PT) = RL;
	    HLeft(right) = *PT;
	    *PT = right;
	    HTFix(HLeft(*PT));
	} else {
	    IPtr     RLL = HLeft(RL);
	    IPtr     RLR = HRight(RL);

	    /*-
	     * left PT [RLL RL RLR] right RR
	     *    ==>  (left PT RLL) RL (RLR right RR)
	     */
	    HLeft(right) = RLR;
	    HRight(*PT) = RLL;
	    HRight(RL) = right;
	    HLeft(RL) = *PT;
	    *PT = RL;
	    HTFix(HLeft(*PT));
	    HTFix(HRight(*PT));
	}
    }
    HTFix(*PT);
}


void    mcNSHWith(x, PT)
    IPtr     x,
           *PT;
{
    ENTER;
    int     h = mcHash(x, true);
    Reg     (temp);

    if (Kind(x) == Undefined || Kind(*PT) == Undefined)
	*PT = OM;		       /* Strictness condition */
    else if (*PT == NilHT)
	*PT = Singleton(x);	       /* Singleton result */
    else if (Kind(*PT) != HTree) {     /* Single bin */
	int     T_hash = mcHash(*PT, true);
	if (h == T_hash) {
	    *PT = Add_To_Bin(x, *PT);  /* Same bin */

	} else if (h < T_hash) {       /* Different bin */
	    R(temp) = Singleton(x);
	    *PT = mcHCat(R(temp), *PT);
	} else {		       /* Different bin */
	    R(temp) = Singleton(x);
	    *PT = mcHCat(*PT, R(temp));
	}
    } else if (h <= mcHMax(HLeft(*PT))) {
	mcNSHWith(x, &HLeft(*PT));     /* Belongs in Left */
	mcNSBal(PT);

    } else {
	mcNSHWith(x, &HRight(*PT));    /* Belongs in Right */
	mcNSBal(PT);
    }

    LEAVE;
}
