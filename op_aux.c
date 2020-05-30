#include "common.h"
/* $Header: op_aux.c,v 3.2 90/04/25 16:23:40 gary Locked $ */
#include "instr.h"
#include "op.h"
#include "manager.h"
#include "symbol.h"
#include "micro.h"
#include "emitter.h"
/* Because these are ISETL operations, the arguments are on the ISETL
 * stack, not passed as C arguments.  Comments refer to the arguments
 * on the stack.  They are correctly accessed by the ENTER macros.
 * See op.h for an explanation of ENTER, LEAVE, R(X_Op).
 * See instr.h for further description of ISETL operations.
 */


/* (ref,val) ref := val */
void    opAssign()
{
    NO_BIN_ENTER;
    if (trace_debug) {
	mcPrint(R(Left_Op), stdout, verbose);
	show(" := ", stdout);
	mcPrint(R(Right_Op), stdout, verbose);
	mcFlush(stdout);
    }
    mcAssign(R(Left_Op), R(Right_Op));
    BIN_NO_RESULT;
}


/* (x):x,x  Duplicates the top of stack */
void    opDup()
{
    Reg     (ans);

    R(ans) = R(Top - 1);
}


/* () Fetches var index and extends Env */
void    opEnv_Extend()
{
    C_Ref   var;

    var = Get_Ref;

    Environment = mcCons(Nil, Environment);
    Info(Environment) = New_Block(Assoc, 0);
    Var_Index(Info(Environment)) = var;
    TRACE_VARS("Extend by", var);
}


/* ()  Removes an assoc from Env */
void    opEnv_Pop()
{
    Environment = Next(Environment);
}



/* (ref):*   De-reference top of stack */
void    opLoad()
{
    UNARY_ENTER;
    C_Ref   id;

    assert(Kind(R(Only_Op)) == Ref);
    id = Var_Index(Var_Assoc(R(Only_Op)));
    TRACE_VARS("Loading", id);
    if (trace_debug) {
	mcprintf(" = ",0,0);
	mcPrint(Var(R(Only_Op)), stdout, verbose);
	mcFlush(stdout);
    }
    R(ans) = Var(R(Only_Op));
    switch (Kind(R(ans))) {
      case Tuple:
      case Set:
      case Closure:
      case String:
      case Predef:
	if (Watched(id)) {
	    Id_Watched(R(ans)) = id;
	} else {
	    Id_Watched(R(ans)) = 0;
	}
    }
    UNARY_LEAVE;
}


/* ():Atom Push a new atom */
void    opPAtom()
{
    Reg     (new);

    R(new) = New_Block(Atom, 0);
}


/* ():ref Fetches var indx and pushes ref */
void    opPRef()
{
    C_Ref   i;
    Reg     (new);

    R(new) = New_Block(Ref, 0);
    i = Get_Ref;
    Var_Assoc(R(new)) = Get_Assoc(i);
}

/* (list of str and int): ref */
void opLookUp() {
    UNARY_ENTER;
    Reg(name);
    Reg(item);
    C_Ref   i;
    char    text[20];

    R(ans) = New_Block(Ref, 0);
    R(name) = New_String("");

    for (R(Only_Op) = mcStart(R(Only_Op));
	 mcNext(&R(item), &R(Only_Op), true);
	) {
            switch( Kind(R(item)) ) {
              case String: break;
              case Integer:
                sprintf(text, "%d", Int_Value(R(item)));
                R(item) = New_String(text);
                break;
              default: RT_ERROR("@ requires strings and small integers");
            }
	    R(name) = mcStr_Concat(R(name), R(item));
	}

    i = find_id(Str_Value(R(name)));

    Var_Assoc(R(ans)) = Get_Assoc(i);
    UNARY_LEAVE;
}

/* (list): list	 Reverses list */
void    opRev()
{
    UNARY_ENTER;
    R(ans) = mcRev(R(Only_Op));
    UNARY_LEAVE;
}


/* (x,y):y,x  Swaps top two stack elements */
void    opSwap()
{
    IPtr     t;

    t = R(Top);
    R(Top) = R(Top - 1);
    R(Top - 1) = t;
}



/* (low,high):List	low..high	 */
void    opCnt2()
{
    BIN_ENTER;

    switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
      case Pair(Integer, Integer):
	R(ans) = mcCnt(Int_Value(R(Left_Op)),
		       Int_Value(R(Right_Op)),
		       (C_Int) 1);
	break;
      case Pair(Integer, Bignum):
      case Pair(Bignum, Integer):
      case Pair(Bignum, Bignum):
	R(Left_Op) = mpForce(R(Left_Op));
	R(Right_Op) = mpForce(R(Right_Op));
	R(ans) = mpCnt(R(Left_Op), R(Right_Op), BigOne);
	break;

      case Pair(String,String): {
	int start, stop;
	char text[2];
	text[1] = 0;

	if( mcLen(R(Left_Op)) != 1 || mcLen(R(Right_Op)) != 1 ) {
	    RT_ERROR("String range over single chars only");
	}
	start = Str_Value(R(Left_Op))[0];
	stop  = Str_Value(R(Right_Op))[0];
	R(ans)= Nil;
	for( ; start <= stop; stop-- ) {
	    R(ans) = mcCons(Nil, R(ans));
	    text[0] = stop;
	    Info(R(ans)) = New_String(text);
	}
      } break;
      default:
	RT_ERROR("Bad args in low..high");
    }

    BIN_LEAVE;
}



Bool is_num(p) IPtr p; {
    return Kind(p)==Integer || Kind(p)==Real || Kind(p)==Bignum ||
	Kind(p)==Rational;
}

Bool is_big(p) IPtr p; {
    return Kind(p)==Integer || Kind(p)==Bignum;
}

Bool is_rat(p) IPtr p; {
    return Kind(p)==Integer || Kind(p)==Bignum || Kind(p)==Rational;
}



/* (1st,2nd,3rd): List	1st,2nd..3rd	 */
void    opCnt3()
{
    TRI_ENTER;

    if (Kind(R(First_Op)) == Integer
	&& Kind(R(Second_Op)) == Integer
	&& Kind(R(Third_Op)) == Integer) {

	R(ans) = mcCnt(Int_Value(R(First_Op)),
		       Int_Value(R(Third_Op)),
		       Int_Value(R(Second_Op))
		       - Int_Value(R(First_Op)));
    } else if (
	is_big(R(First_Op)) && is_big(R(Second_Op)) && is_big(R(Third_Op))
	) {
	Reg     (diff);

	R(First_Op) = mpForce(R(First_Op));
	R(Second_Op) = mpForce(R(Second_Op));
	R(Third_Op) = mpForce(R(Third_Op));
	R(diff) = mpSub(R(Second_Op), R(First_Op));
	R(diff) = mpForce(R(diff));
	R(ans) = mpCnt(R(First_Op), R(Third_Op), R(diff));
    } else if (
	is_rat(R(First_Op)) && is_rat(R(Second_Op)) && is_rat(R(Third_Op))
	) {
	Reg     (diff);

	R(diff) = RatSub(R(Second_Op), R(First_Op));
	R(diff) = RatForce(R(diff));
	R(ans) = RatCnt(R(First_Op), R(Third_Op), R(diff));
    } else if(
	is_num(R(First_Op)) && is_num(R(Second_Op)) && is_num(R(Third_Op))
	) {
	C_Real start, second, step, stop;
	int cnt;
	R(First_Op) = mpFloat(R(First_Op));
	start       = Real_Value(R(First_Op));
	second      = Real_Value(mpFloat(R(Second_Op)));
	stop        = Real_Value(mpFloat(R(Third_Op)));
	step        = second-start;

	if( step == 0 ) {
	    RT_ERROR("Zero step size in floating point low,next..high");
	}
	cnt = floor((stop-start)/step + 0.0000001);

	R(ans) = Nil;

	for (; cnt >= 0; cnt--) {
	    R(ans) = mcCons(Nil, R(ans));
	    Info(R(ans)) = New_Real(start + cnt*step);
	}
    } else {
	RT_ERROR("Bad args in low,next..high");
    }

    TRI_LEAVE;
}



/* (x,L):List cons(x,L) */
void    opCons()
{
    BIN_ENTER;
    assert(Kind(R(Right_Op)) == List);

    R(ans) = mcCons(R(Left_Op), R(Right_Op));
    BIN_LEAVE;
}



/* ()	No operation */
void    opNoOp()
{
}


/* ():List Pushes Nil */
void    opPNil()
{
    Reg     (new);

    R(new) = Nil;
}



/* ():Boolean Pushes True */
void    opPTrue()
{
    Reg     (new);

    R(new) = ITRUE;
}

/* ():Boolean Pushes False */
void    opPFalse()
{
    Reg     (new);

    R(new) = IFALSE;
}

/* ():Undefined Pushes OM */
void    opPOM()
{
    Reg     (new);

    R(new) = OM;
}

/* ():Undefined Pushes NP */
void    opPNP()
{
    Reg     (new);

    R(new) = NonPrint;
}



/* (x)	Ignores x.  Deletes it from Stack */
void    opPop()
{
    mcPop();
}

/* forward static declarations */
IPtr mcIterLoad(PTR);
IPtr mcFetchVal(PTR);
IPtr mcFetchTuple(PTR);

/* (iter):*   Leave value of iters on stack */
void    opIter_Load()
{
    UNARY_ENTER;

    assert(Kind(R(Only_Op)) == Iter);
    R(ans) = mcIterLoad(R(Only_Op));
    if( Iter_Rest(R(Only_Op)) == Nil ) {   /* return single element */
	R(ans) = Info(R(ans));
    } else {				   /* convert list to tuple */
	R(ans) = New_Tuple(R(ans), def_orig);
    }

    UNARY_LEAVE;
}


/* Innermost iterator at front of list */
static IPtr mcIterLoad(p) IPtr p; {
    ENTER;
    Reg(ans);
    Reg(tmp);

    R(ans) = Nil;
    for( ; p != Nil; p = Iter_Rest(p) ) {
	R(tmp) = mcFetchVal(Iter_LHS(p));
	R(ans) = mcCons(R(tmp),R(ans));
    }
    LEAVE R(ans);
}

static IPtr mcFetchVal(p) IPtr p; {
    ENTER;
    Reg(ans);
    Reg(tmp);
    Reg(list);
    Reg(item);

    switch( Kind(p) ) {
      case Tuple:  
	R(ans) = Nil;
	R(list) = mcStart(Tuple_Value(p));
	while( mcNext(&R(item), &R(list), true) ) {
	    R(tmp) = mcFetchVal(R(item));
	    R(ans) = mcCons(R(tmp),R(ans));
	}
	R(ans) = mcRev(R(ans));
	R(ans) = New_Tuple(R(ans), def_orig);
	break;
      case Ref:
	R(ans) = Var(p);
	break;
      case Undefined:
	break;
      default: 
	assert(0);
    }
    LEAVE R(ans);
}

