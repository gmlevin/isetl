/* $Header: mc_aux.c,v 3.1 90/04/25 16:23:04 gary Locked $ */
#include "common.h"
#include "micro.h"
#include "manager.h"


/* (val, sels, new_val):IPtr val modified at sels to be new_val */
IPtr     mcModify(val, sels, new_val)
    IPtr     val,
            sels,
            new_val;
{
    ENTER;
    Reg     (ans);

    if (sels == Nil)
	R(ans) = new_val;

    else
	switch (Pair(Kind(val), Sel_Is(sels))) {
	  case Pair(String, Sel_Slice):
	  case Pair(String, Sel_SMap):{
		Idx     at,
		        to,
		        len;
		Reg     (front);
		Reg     (mid);
		Reg     (tail);

		len = mcLen(val);

		/* get at */
		if (At_Index(sels) == Nil)
		    at = 1;
		else if (!mpToInt(At_Index(sels), &at))
		    BAD_MAP(NULL, val, sels);

		/* get to */
		if (Sel_Is(sels) == Sel_SMap) {
		    /* str(i) == str(i..i) */
		    to = at;
		} else {
		    if (To_Index(sels) == Nil)
			to = len;
		    else if (!mpToInt(To_Index(sels), &to))
			BAD_MAP(NULL, val, sels);
		}

		/* check bounds */
		if (at > to + 1)
		    BAD_MAP(NULL, val, sels);
		if (at < 1 || to > len)
		    BAD_MAP(NULL, val, sels);

		R(front) = mcSSlice(val, (Idx) 1, at - 1);
		R(mid) = mcSSlice(val, at, to);
		R(tail) = mcSSlice(val, to + 1, len);

		R(ans) = mcModify(R(mid), Sel_Next(sels), new_val);
		if (Kind(R(ans)) != String)
		    RT_ERROR("RHS in string slice assignment must be string");
		R(ans) = mcStr_Concat(R(ans), R(tail));
		R(ans) = mcStr_Concat(R(front), R(ans));
	    } break;

	  case Pair(Tuple, Sel_SMap):{
	        Idx     orig = Tuple_Origin(val);
		Idx     at;
		Reg     (temp);

		if (!mpToInt(At_Index(sels), &at))
		    BAD_MAP(NULL, val, sels);

		if (at < orig)
		    BAD_MAP(NULL, val, sels);

		val = Tuple_Value(val);

		R(temp) = mcModify(mcAccess(val, at-orig),
				   Sel_Next(sels), new_val);
		R(ans) = mcUpdate(val, at-orig, R(temp));
		R(ans) = New_Tuple(R(ans), orig);
	    } break;

	  case Pair(Tuple, Sel_Slice):{
		Reg     (front);
		Reg     (mid);
		Reg     (tail);
		Idx     orig = Tuple_Origin(val);
		Idx     at,
		        to,
		        len;

		len = mcLen(val);
		val = Tuple_Value(val);

		if (At_Index(sels) == Nil)
		    at = orig;
		else if (!mpToInt(At_Index(sels), &at))
		    BAD_MAP(NULL, val, sels);

		if (To_Index(sels) == Nil)
		    to = len;
		else if (!mpToInt(To_Index(sels), &to))
		    BAD_MAP(NULL, val, sels);

		if (at > to + 1)
		    BAD_MAP(NULL, val, sels);
		if (at < orig)
		    BAD_MAP(NULL, val, sels);

		R(front) = mcTSlice(val, (Idx) 0, at - orig-1);
		R(mid) = mcTSlice(val, at-orig, to-orig);
		R(tail) = mcTSlice(val, to-orig + 1, len-1);

		R(mid) = New_Tuple(R(mid),orig);
		R(mid) = mcModify(R(mid), Sel_Next(sels), new_val);

		if (Kind(R(mid)) != Tuple)
		    RT_ERROR("RHS in tuple slice assignment must be tuple");

		R(ans) = mcCat(Tuple_Value(R(mid)), R(tail));
		R(ans) = mcCat(R(front), R(ans));
		R(ans) = New_Tuple(R(ans), orig);
	    } break;

	  case Pair(Closure, Sel_SMap):{
		Reg     (temp);

		R(ans) = New_Block(Closure, 0);
		Cl_Code(R(ans)) = Cl_Code(val);
		Cl_Env(R(ans)) = Cl_Env(val);
		Cl_SRC(R(ans)) = Cl_SRC(val);
		Cl_Cnt(R(ans)) = Cl_Cnt(val);

		if (Sel_Next(sels) == Nil) {
		    R(temp) = new_val;
		} else {
		    RT_ERROR("Only one level of selection allowed");
		}

		Cl_Override(R(ans)) = mcDel_Image(Cl_Override(val),
						  At_Index(sels));
		/* Pair temp with index */
		R(temp) = mcTwo_Tuple(At_Index(sels), R(temp));
		Cl_Override(R(ans)) = mcHWith(R(temp),
					      Cl_Override(R(ans)));
	    } break;

	  case Pair(Set, Sel_SMap):{
		Reg     (temp);

		if (Sel_Next(sels) == Nil) {
		    R(temp) = new_val;
		} else {
		    R(temp) = mcImages(Set_Value(val), At_Index(sels));
		    if (mcHTLen(R(temp)) != 1)
			BAD_MAP("Multiple images", val, sels);

		    R(temp) = mcHAny(R(temp));
		    R(temp) = mcModify(R(temp), Sel_Next(sels), new_val);
		}

		R(ans) = mcDel_Image(Set_Value(val), At_Index(sels));
		if (R(temp) != OM) {
		    R(temp) = mcTwo_Tuple(At_Index(sels), R(temp));
		    R(ans) = mcHWith(R(temp), R(ans));
		}
		R(ans) = New_Set(R(ans));
	    } break;

	  case Pair(Set, Sel_MMap):{
		Reg     (temp);
		Reg     (item);

		if (Sel_Next(sels) == Nil) {
		    R(temp) = new_val;
		} else {
		    R(temp) = mcImages(Set_Value(val), At_Index(sels));
		    R(temp) = New_Set(R(temp));
		    R(temp) = mcModify(R(temp), Sel_Next(sels), new_val);
		}

		R(ans) = mcDel_Image(Set_Value(val), At_Index(sels));
		if (R(temp) != OM) {
		    if (Kind(R(temp)) != Set)
			RT_ERROR("RHS in mmap assignment must be set");

		    for (R(temp) = mcStart(Set_Value(R(temp)));
			 mcNext(&R(item), &R(temp), true);
			) {
			R(item) = mcTwo_Tuple(At_Index(sels), R(item));
			R(ans) = mcHWith(R(item), R(ans));
		    }
		}
		R(ans) = New_Set(R(ans));
	    } break;

	  default:
	    BAD_MAP("in LHS of assignment", val, sels);
	}

    LEAVE   R(ans);
}




/* (string,at,to):string string(at..to) */
IPtr     mcSSlice(str, at, to)
    IPtr     str;
    Idx     at,
            to;
{
    ENTER;
    Reg     (ans);
    C_Str   p;
    int     len;

    assert(Kind(str) == String);

    if (to > strlen(Str_Value(str)))
	RT_ERROR("Slice upper bound too big");
    if (at > to + 1)
	RT_ERROR("Slice lower bound too big");

    len = to - at + 1;
    R(ans) = New_Block(String, len);
    p = Str_Value(R(ans));
    strncpy(p, &(Str_Value(str)[at - 1]), len);
    p[len] = EOS;

    LEAVE   R(ans);
}


/* Delete Assoc pairs from Environment, corresponding to elements in lhs */
void    mcDel_Env(lhs)
    IPtr     lhs;
{
    ENTER;
    Reg     (item);
    Reg     (list);

    switch (Kind(lhs)) {
      case Ref:
	Environment = Next(Environment);
	break;

      case Undefined:
	/* nothing in Environment */
	break;

      case Tuple:
	/* NB: Added to Env in order, hence must pop in rev order */
	R(list) = mcStart(Tuple_Value(lhs));
	while (mcNext(&R(item), &R(list), true)) {
	    mcDel_Env(R(item));
	}
	break;

      default:
	mcprintf("Kind = %d\n", Kind(lhs),0);
	assert(0);
    }
    LEAVE;
}

/* (ref): *, fetch value of ref */
IPtr     mcVal(r)
    IPtr     r;
{
    ENTER;
    Reg     (sels);
    Reg     (ans);

    R(sels) = Sel_List(r);
    R(ans) = Var(r);
    for (; R(sels) != Nil; R(sels) = Sel_Next(R(sels))) {
	switch (Pair(Kind(R(ans)), Sel_Is(R(sels)))) {
	  case Pair(String, Sel_Slice):
	  case Pair(String, Sel_SMap):{
		Idx     at,
		        to,
		        len;

		len = mcLen(R(ans));

		/* get at */
		if (At_Index(R(sels)) == Nil)
		    at = 1;
		else if (!mpToInt(At_Index(R(sels)), &at))
		    BAD_REF(NULL, r);

		/* get to */
		if (Sel_Is(R(sels)) == Sel_SMap) {
		    /* str(i) == str(i..i) */
		    to = at;
		} else {
		    if (To_Index(R(sels)) == Nil)
			to = len;
		    else if (!mpToInt(To_Index(R(sels)), &to))
			BAD_REF(NULL, r);
		}

		/* check bounds */
		if (at > to + 1)
		    BAD_REF(NULL, r);
		if (at < 1 || to > len)
		    BAD_REF(NULL, r);

		R(ans) = mcSSlice(R(ans), at, to);
	    } break;

	  case Pair(Tuple, Sel_SMap):{
		Idx     at;
		Idx	orig = Tuple_Origin(R(ans));

		R(ans) = Tuple_Value(R(ans));

		if (!mpToInt(At_Index(R(sels)), &at))
		    BAD_REF(NULL, r);

		at = Int_Value(At_Index(R(sels)));
		if (at < orig)
		    BAD_REF(NULL, r);

		R(ans) = mcAccess(R(ans), at-orig);
	    } break;

	  case Pair(Tuple, Sel_Slice):{
	        Idx     orig = Tuple_Origin(R(ans));
		Idx     at,
		        to,
		        len;

		len = mcLen(R(ans));
		R(ans) = Tuple_Value(R(ans));

		if (At_Index(R(sels)) == Nil)
		    at = orig;
		else if (!mpToInt(At_Index(R(sels)), &at))
		    BAD_REF(NULL, r);

		if (To_Index(R(sels)) == Nil)
		    to = len;
		else if (!mpToInt(To_Index(R(sels)), &to) == Integer)
		    BAD_REF(NULL, r);

		if (at > to + 1)
		    BAD_REF(NULL, r);
		if (at < orig)
		    BAD_REF(NULL, r);

		R(ans) = mcTSlice(R(ans), at-orig, to-orig);
		R(ans) = New_Tuple(R(ans),orig);
	    } break;

	  case Pair(Set, Sel_SMap):{
		R(ans) = mcImages(Set_Value(R(ans)), At_Index(R(sels)));
		if (mcHTLen(R(ans)) != 1)
		    BAD_REF("multiple images", r);
		R(ans) = mcHAny(R(ans));
	    } break;

	  case Pair(Set, Sel_MMap):{
		R(ans) = mcImages(Set_Value(R(ans)), At_Index(R(sels)));
		R(ans) = New_Set(R(ans));
	    } break;

	  default:
	    BAD_REF(NULL, r);
	}
    }
    LEAVE   R(ans);
}



