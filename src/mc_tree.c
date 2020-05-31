#include "common.h"
/* $Header: mc_tree.c,v 3.1 90/04/25 16:23:22 gary Locked $ */
#include "micro.h"
#include "manager.h"


/* static forward declaration */
IPtr     TSlice(PTR X IDX X IDX);
void    Fold(IDX X PTR ADDR X PTR ADDR);

/* AVL trees ordered by position (length) (origin 0) */

/* tree(i) */
IPtr     mcAccess(tree, i)
    IPtr     tree;
    Idx     i;
{
    ENTER;
    IPtr     p;

    assert(i >= 0);

    if (i >= mcTLen(tree)) {
	p = OM;
    } else {
	p = tree;
	while (Kind(p) == Tree) {
	    if (i < mcTLen(Left(p))) {
		p = Left(p);
	    } else {
		i = i - mcTLen(Left(p));	/* adjust offset  */
		p = Right(p);
	    }
	}
	assert(i == 0);
    }

    LEAVE   p;
}




/* assumes neither is empty */
IPtr     Cat(PTR X PTR);
static IPtr Cat(a, b)
    IPtr     a,
            b;
{
    ENTER;
    Reg     (t1);
    Reg     (t2);
    Reg     (ans);
    int     delta = mcHgt(a) - mcHgt(b);

    if (-max_delta <= delta && delta <= max_delta) {
	R(ans) = New_Tree(a, b);

    } else if (delta > 0) {	       /* a is taller */
	int     subdelta = mcHgt(Left(a)) - mcHgt(Right(a));

	if (subdelta >= 0) {
	    R(t1) = Cat(Right(a), b);
	    R(ans) = New_Tree(Left(a), R(t1));

	} else {
	    R(t1) = New_Tree(Left(a), Left(Right(a)));
	    R(t2) = Cat(Right(Right(a)), b);
	    R(ans) = Cat(R(t1), R(t2));
	}
    } else {			       /* b is taller */
	int     subdelta = mcHgt(Left(b)) - mcHgt(Right(b));

	if (subdelta <= 0) {
	    R(t1) = Cat(a, Left(b));
	    R(ans) = New_Tree(R(t1), Right(b));

	} else {
	    R(t1) = Cat(a, Left(Left(b)));
	    R(t2) = New_Tree(Right(Left(b)), Right(b));
	    R(ans) = Cat(R(t1), R(t2));
	}
    }


    LEAVE   R(ans);
}

IPtr     mcCat(a, b)
    IPtr     a,
            b;
{

    if (a == NilT)
	return b;
    else if (b == NilT)
	return a;
    else
	return Cat(a, b);
}




/* (tree,at,to):tree   tree(at..to)
 * assumes
 * 	0 <= at <= to && at < len
 * 	tree is not empty
 */
static IPtr TSlice(tree, at, to)
    IPtr     tree;
    Idx     at,
            to;
{
    ENTER;
    Reg     (left);
    Reg     (right);
    Reg     (ans);
    Idx     left_len;
    Idx     len = mcTLen(tree);

    if (at == 0 && to >= len-1) {
	R(ans) = tree;

    } else {
	left_len = mcTLen(Left(tree));

	if (to < left_len) {
	    R(ans) = TSlice(Left(tree), at, to);

	} else if (at >= left_len) {
	    R(ans) = TSlice(Right(tree), at - left_len, to - left_len);

	} else {
	    R(left) = TSlice(Left(tree), at, left_len-1);
	    R(right) = TSlice(Right(tree), (Idx) 0, to - left_len);
	    R(ans) = mcCat(R(left), R(right));
	}
    }
    LEAVE   R(ans);
}


/* (tree,at,to):tree  tree(at..to)
 * assumes 0 <= at
 */
IPtr     mcTSlice(tree, at, to)
    IPtr     tree;
    Idx     at,
            to;
{
    ENTER;
    Reg     (ans);
    Idx     len = mcTLen(tree);

    if (at >= len || to < at)
	R(ans) = NilT;
    else
	R(ans) = TSlice(tree, at, to);

    LEAVE   R(ans);
}




/* (x:*,T:Tree):bool x in? T */
Bool    mcIn_Tree(x, T)
    IPtr     x,
            T;
{
    if (T == NilT)
	return false;
    if (Kind(T) != Tree)
	return mcEq(x, T);
    return mcIn_Tree(x, Left(T)) || mcIn_Tree(x, Right(T));
}


/* (tree,int,x):tree tree(int) := x (origin 0) */
IPtr     mcUpdate(tree, i, x)
    IPtr     tree,
            x;
    Idx     i;
{
    ENTER;
    Reg     (ans);
    Idx     len = mcTLen(tree);

    assert(i >= 0);

    if (i >= len) {		       /* R(ans) = tree + [om,om,...,om,x] */
	R(ans) = x;
	i--;
	while (i >= len) {
	    R(ans) = mcCat(OM, R(ans));
	    i--;
	}
	R(ans) = mcCat(tree, R(ans));

    } else if (Kind(tree) != Tree) {   /* leaf */
	assert(i == 0);
	R(ans) = x;
    } else {
	Idx     left_len = mcTLen(Left(tree));

	if (i < left_len) {
	    R(ans) = mcUpdate(Left(tree), i, x);
	    R(ans) = New_Tree(R(ans), Right(tree));
	} else {
	    R(ans) = mcUpdate(Right(tree), i - left_len, x);
	    R(ans) = New_Tree(Left(tree), R(ans));
	}
    }


    LEAVE   R(ans);
}




/* Commands for iterating through trees and lists
 *
 * Works by using a worklist of trees to be traversed.
 */


/* Returns a structure for iterating through x
 */
IPtr     mcStart(x)
    IPtr     x;
{
    if (Kind(x) == List)
	return x;
    if (x == NilT)
	return Nil;
    if (x == NilHT)
	return Nil;
    return mcCons(x, Nil);
}


/* Assign *x the next value from *work, deleting it from *work
 * Return true if a value is returned, false ow.
 * *work == Nil when done .
 * *x and *work must both be alive (accessible).
 */
Bool    mcNext(x, work, ordered)
    IPtr    *x,
           *work;
    Bool    ordered;
{
    if (*work == Nil) {
	*x = OM;
	return false;
    }
    *x = Info(*work);
    *work = Next(*work);

    while (true) {		       /* While x is not atomic, split x,
				        * pushing part back on work */
	if (Kind(*x) == Tree) {
	    *work = mcCons(Right(*x), *work);
	    *x = Left(*x);
	} else if (Kind(*x) == HTree) {
	    if (ordered || rrand() < 0.5) {
		*work = mcCons(HRight(*x), *work);
		*x = HLeft(*x);
	    } else {
		*work = mcCons(HLeft(*x), *work);
		*x = HRight(*x);
	    }
	} else if (Kind(*x) == List) {
	    if (*x != Nil) {
		*work = mcCons(Next(*x), *work);
		*x = Info(*x);
	    } else if (*work != Nil) { /* x is empty, get more from work */
		*x = Info(*work);
		*work = Next(*work);
	    } else {		       /* no more work, quit */
		*x = OM;
		return false;
	    }
	} else			       /* atomic x */
	    break;
    }
    return true;
}




/* Advances *p through a list, assigning to *t a tree of max size s.
 * assumes that *p and *t are active.
 * Converts a list to a tree in linear time.
 */
static void Fold(s, t, p)
    Idx     s;
    IPtr    *t,
           *p;
{
    ENTER;
    Reg     (left);
    Reg     (right);

    if (*p == Nil) {
	*t = NilT;
    } else if (s == 1) {
	*t = Info(*p);
	*p = Next(*p);
    } else {
	Fold(s / 2, &R(left), p);
	Fold(s - s / 2, &R(right), p);
	*t = mcCat(R(left), R(right));
    }
    LEAVE;
}


/* (x): Return tuple with values in x */
IPtr     mcList_Tree(x)
    IPtr     x;
{
    ENTER;
    Reg     (tree);
    Reg     (ans);

    Fold((Idx) 1, &R(ans), &x);

    while (x != Nil) {
	Fold(mcTLen(R(ans)), &R(tree), &x);
	R(ans) = mcCat(R(ans), R(tree));
    }

    LEAVE   R(ans);
}
