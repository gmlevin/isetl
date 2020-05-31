#include "common.h"
/* $Header: op_logic.c,v 3.1 90/04/25 16:23:52 gary Locked $ */
#include "instr.h"
#include "op.h"
#include "manager.h"
#include "micro.h"
#include "emitter.h"
/* Because these are ISETL operations, the arguments are on the ISETL
 * stack, not passed as C arguments.  Comments refer to the arguments
 * on the stack.  They are correctly accessed by the ENTER macros.
 * See op.h for an explanation of ENTER, LEAVE, R(X_Op).
 * See instr.h for further description of ISETL operations.
 */


/* (a,b):*  a and b */
void    opAnd()
{
    BIN_ENTER;

    switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
      case Pair(Boolean, Boolean):
	R(ans) = New_Boolean(Bool_Value(R(Left_Op))
			     && Bool_Value(R(Right_Op)));
	break;
      default:
	BAD_ARGS("and", R(Left_Op), R(Right_Op));
    }

    BIN_LEAVE;
}




/* (a,b):*  a or b */
void    opOr()
{
    BIN_ENTER;

    switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
      case Pair(Boolean, Boolean):
	R(ans) = New_Boolean(Bool_Value(R(Left_Op))
			     || Bool_Value(R(Right_Op)));
	break;
      default:
	BAD_ARGS("or", R(Left_Op), R(Right_Op));
    }

    BIN_LEAVE;
}



void    opNot()
{
    UNARY_ENTER;
    switch (Kind(R(Only_Op))) {
      case Boolean:
	R(ans) = New_Boolean(!Bool_Value(R(Only_Op)));
	break;
      default:
	BAD_ARGS("not", NULL, R(Only_Op));
    }
    UNARY_LEAVE;
}

void    opImpl()
{
    BIN_ENTER;

    switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
      case Pair(Boolean, Boolean):
	R(ans) = New_Boolean(!Bool_Value(R(Left_Op))
			     || Bool_Value(R(Right_Op)));
	break;

      default:
	BAD_ARGS("impl", R(Left_Op), R(Right_Op));
    }

    BIN_LEAVE;
}




void    opEq()
{
    BIN_ENTER;
    R(ans) = New_Boolean(mcEq(R(Left_Op), R(Right_Op)));
    BIN_LEAVE;
}

void    opNe()
{
    BIN_ENTER;
    R(ans) = New_Boolean(!mcEq(R(Left_Op), R(Right_Op)));
    BIN_LEAVE;
}

void    opGe()
{
    BIN_ENTER;
    R(ans) = New_Boolean(!mcLt(R(Left_Op), R(Right_Op)));
    BIN_LEAVE;
}

void    opGt()
{
    BIN_ENTER;
    R(ans) = New_Boolean(mcLt(R(Right_Op), R(Left_Op)));
    BIN_LEAVE;
}

void    opLe()
{
    BIN_ENTER;
    R(ans) = New_Boolean(!mcLt(R(Right_Op), R(Left_Op)));
    BIN_LEAVE;
}

void    opLt()
{
    BIN_ENTER;
    R(ans) = New_Boolean(mcLt(R(Left_Op), R(Right_Op)));
    BIN_LEAVE;
}



void    opNotin()
{
    opIn();
    opNot();
}

void    opIn()
{
    BIN_ENTER;
    switch (Kind(R(Right_Op))) {
      case Set:
	R(ans) = New_Boolean(
			     mcIn_HTree(R(Left_Op),
					Set_Value(R(Right_Op))));
	break;

      case Tuple:
	R(ans) = New_Boolean(
			     mcIn_Tree(R(Left_Op),
				       Tuple_Value(R(Right_Op))));
	break;

      case String:
	if (Kind(R(Left_Op)) != String)
	    BAD_ARGS("in", R(Left_Op), R(Right_Op));
	R(ans) = New_Boolean(
			     mcSubstr(Str_Value(R(Left_Op)),
				      Str_Value(R(Right_Op))));
	break;

      default:
	BAD_ARGS("in", R(Left_Op), R(Right_Op));
    }
    BIN_LEAVE;
}


void    opSubset()
{
    BIN_ENTER;
    switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
      case Pair(Set, Set):
	R(ans) = New_Boolean(mcSubset(Set_Value(R(Left_Op)),
				      Set_Value(R(Right_Op))));
	break;

      default:
	BAD_ARGS("subset", R(Left_Op), R(Right_Op));
    }
    BIN_LEAVE;
}

