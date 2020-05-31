#include "common.h"
/* $Header: op_data.c,v 3.2 90/04/25 16:23:46 gary Locked $ */
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

/* (a,b):*  a less b */
void    opLess()
{
    BIN_ENTER;

    switch (Kind(R(Left_Op))) {
      case Set:{
	    R(ans) = mcLess(R(Right_Op), R(Left_Op));
	} break;

      default:
	BAD_ARGS("less", R(Left_Op), R(Right_Op));
    }

    BIN_LEAVE;
}




/* (a,b):*  a with b */
void    opWith()
{
    BIN_ENTER;

    switch (Kind(R(Left_Op))) {
      case Tuple:{
	    R(ans) = mcCat(Tuple_Value(R(Left_Op)), R(Right_Op));
	    R(ans) = New_Tuple(R(ans), Tuple_Origin(R(Left_Op)));
	} break;

      case Set:{
	    R(ans) = mcWith(R(Right_Op), R(Left_Op));
	} break;

      default:
	BAD_ARGS("with", R(Left_Op), R(Right_Op));
    }

    BIN_LEAVE;
}

/* (x):Tuple Coerces x to a Tuple */
void    opMk_Tuple()
{
    UNARY_ENTER;
    R(ans) = New_Tuple(R(Only_Op), def_orig);
    UNARY_LEAVE;
}

/* (x):Set Coerces x to a Set */
void    opMk_Set()
{
    UNARY_ENTER;
    R(ans) = New_Set(R(Only_Op));
    UNARY_LEAVE;
}




void    opCount()
{
    UNARY_ENTER;
    R(ans) = New_Integer((WORKING) mcLen(R(Only_Op)));
    UNARY_LEAVE;
}


static void    Add_Sel(PTR X PTR);


/* (ref1,ref2) take ref1 from ref2 */
void    opFrom()
{
    NO_BIN_ENTER;
    mcFrom(R(Left_Op), R(Right_Op));
    BIN_NO_RESULT;
}

/* (ref1,ref2) take ref1 fromb ref2 */
void    opFromb()
{
    NO_BIN_ENTER;
    mcFromb(R(Left_Op), R(Right_Op));
    BIN_NO_RESULT;
}

/* (ref1,ref2) take ref1 frome ref2 */
void    opFrome()
{
    NO_BIN_ENTER;
    mcFrome(R(Left_Op), R(Right_Op));
    BIN_NO_RESULT;
}

/*
 * (ref,x):ref ref(x), (val,x):val val(x) (works for pre-def),
 * (closure,x):(ret-closure,x) and PC and Environment modified to closure
 * so that continued execution will result in closure(x) eventually.
 *
 * The unusual form is to accomodate the case where two values are left on
 * the stack.
 */

#define YIELDS	\
	if( id != 0 ) { \
	    show("! Yields: ", stdout); \
	    mcPrint( R(ans), stdout , verbose ); \
	    mcFlush(stdout); \
	} else


void    opSMap()
{
    BIN_ENTER;
    C_Ref   id;

    switch (Kind(R(Left_Op))) {
      case Predef:
      case Closure:
      case Tuple:
      case String:
      case Set:
	id = Id_Watched(R(Left_Op));
	break;
      default:
	id = 0;
    }


    if (id != 0) {
	show("! Evaluate: ", stdout);
	show(Id_String(id), stdout);
	show("(", stdout);
	mcPrint(R(Right_Op), stdout, verbose);
	show(")", stdout);
	mcFlush(stdout);
    }
    switch (Kind(R(Left_Op))) {
      case Predef:{
	    /* make args (Right_Op) a list */
	    if (Kind(R(Right_Op)) != List) {
		R(Right_Op) = mcCons(R(Right_Op), Nil);
	    }
	    R(ans) = mcPdCall(R(Left_Op), R(Right_Op));
	} break;


      case Closure:{
	    R(ans) = OM;
	    if (Cl_Override(R(Left_Op)) != Nil) {
		/* Look-up image */
		Reg     (args);

		R(args) = R(Right_Op);
		if (Kind(R(args)) == List) {
		    R(args) = New_Tuple(R(args), (Idx)1);
		}
		R(ans) = mcImages(Cl_Override(R(Left_Op)), R(args));
		if (mcHTLen(R(ans)) == 1) {
		    YIELDS;
		    BIN_LEAVE;
		}
	    }

	    /* No override value */
	    /* make args (Right_Op) a list */
	    if (Kind(R(Right_Op)) != List) {
		R(Right_Op) = mcCons(R(Right_Op), Nil);
	    }

	    /* Push Return Closure */
	    R(ans) = New_Block(Closure, 0);
	    Cl_Env(R(ans)) = Environment;
	    Cl_Code(R(ans)) = CNext(EP_Ptr(PC));
	    Cl_SRC(R(ans)) = Last_Line;
	    Id_Watched(R(ans)) = id;

	    call_level++;
	    GO_TO(Cl_Code(R(Left_Op)));
	    Environment = Cl_Env(R(Left_Op));
	    assert(Kind(EP_Ptr(PC)) == Join);

	    R(Left_Op) = R(ans);
	    Top = Old_Top;
	    /* No BIN_LEAVE to leave two values on stack */
	    return;
	} break;

      case Ref:{
	    Reg     (select);

	    R(select) = New_Block(Sel, 0);
	    if (Kind(R(Right_Op)) == List) {
		R(Right_Op) = New_Tuple(R(Right_Op),(Idx)1);
	    }
	    At_Index(R(select)) = R(Right_Op);
	    Sel_Is(R(select)) = Sel_SMap;
	    R(ans) = R(Left_Op);
	    Add_Sel(R(select), R(ans));
	    BIN_LEAVE;
	} break;

	/* Select the element from R(Left_Op) */
      case Tuple:{
	    Idx     i;
	    Idx     orig = Tuple_Origin(R(Left_Op));

	    if (!mpToInt(R(Right_Op), &i))
		BAD_SMAP(NULL, R(Left_Op), R(Right_Op));

	    if (i < orig)
		BAD_SMAP(NULL, R(Left_Op), R(Right_Op));
	    R(ans) = mcAccess(Tuple_Value(R(Left_Op)),  i-orig);
	} break;

      case String:{
	    Idx     i;
	    char    singleton[2];

	    if (!mpToInt(R(Right_Op), &i))
		BAD_SMAP(NULL, R(Left_Op), R(Right_Op));

	    if (i < 1)
		BAD_SMAP(NULL, R(Left_Op), R(Right_Op));

	    if (i > mcLen(R(Left_Op))) {
		R(ans) = OM;
	    } else {
		singleton[1] = EOS;
		singleton[0] = Str_Value(R(Left_Op))[i - 1];
		R(ans) = New_String(singleton);
	    }
	} break;

      case Set:{
	    Idx     i;

	    if (Kind(R(Right_Op)) == List) {
		R(Right_Op) = New_Tuple(R(Right_Op),(Idx)1);
	    }
	    if( R(Right_Op) == OM ) {
		BAD_SMAP("", R(Left_Op), R(Right_Op));
	    }
	    R(ans) = mcImages(Set_Value(R(Left_Op)), R(Right_Op));
	    i = mcHTLen(R(ans));
	    if (i == 0)
		R(ans) = OM;
	    else if (i == 1)
		R(ans) = R(ans);
	    else {
		BAD_SMAP("multiple images", R(Left_Op), R(Right_Op));
	    }
	} break;

      default:
	BAD_SMAP(NULL, R(Left_Op), R(Right_Op));
    }
    YIELDS;
    BIN_LEAVE;
}


/*
 * (ref,x):ref ref{x}, (val,x):val val{x}
 */
void    opMMap()
{
    BIN_ENTER;
    C_Ref   id;

    switch (Kind(R(Left_Op))) {
      case Tuple:
      case String:
      case Set:
	id = Id_Watched(R(Left_Op));
	break;
      default:
	id = 0;
    }

    if (id != 0) {
	show("! Evaluate: ", stdout);
	show(Id_String(id), stdout);
	show("{", stdout);
	mcPrint(R(Right_Op), stdout, verbose);
	show("}", stdout);
	mcFlush(stdout);
    }
    switch (Kind(R(Left_Op))) {
      case Ref:{
	    Reg     (select);

	    R(select) = New_Block(Sel, 0);
	    if (Kind(R(Right_Op)) == List) {
		R(Right_Op) = New_Tuple(R(Right_Op),(Idx)1);
	    }
	    At_Index(R(select)) = R(Right_Op);
	    Sel_Is(R(select)) = Sel_MMap;
	    R(ans) = R(Left_Op);
	    Add_Sel(R(select), R(ans));
	    BIN_LEAVE;
	} break;

	/* Select the element from R(Left_Op) */
      case Tuple:{
	    Idx     i;
	    Idx     orig = Tuple_Origin(R(Left_Op));

	    if (!mpToInt(R(Right_Op), &i))
		BAD_MMAP(NULL, R(Left_Op), R(Right_Op));

	    if (i < orig)
		BAD_MMAP(NULL, R(Left_Op), R(Right_Op));
	    R(ans) = mcAccess(Tuple_Value(R(Left_Op)),  i-orig);
	} break;

      case String:{
	    Idx     i;
	    char    singleton[2];

	    if (!mpToInt(R(Right_Op), &i))
		BAD_MMAP(NULL, R(Left_Op), R(Right_Op));

	    if (i < 1)
		BAD_MMAP(NULL, R(Left_Op), R(Right_Op));

	    if (i > mcLen(R(Left_Op))) {
		R(ans) = OM;
	    } else {
		singleton[1] = EOS;
		singleton[0] = Str_Value(R(Left_Op))[i - 1];
		R(ans) = New_String(singleton);
	    }
	} break;

      case Set:{
	    if (Kind(R(Right_Op)) == List) {
		R(Right_Op) = New_Tuple(R(Right_Op),(Idx)1);
	    }
	    R(ans) = mcImages(Set_Value(R(Left_Op)), R(Right_Op));
	} break;

      default:
	BAD_MMAP(NULL, R(Left_Op), R(Right_Op));
    }
    if( R(ans) != OM ) R(ans) = New_Set(R(ans));
    else               R(ans) = New_Set(Nil);
    YIELDS;
    BIN_LEAVE;
}


/* add select to end of Sel_List(ref) */
static void Add_Sel(select, ref)
    IPtr     select,
            ref;
{
    if (Sel_List(ref) == Nil) {
	Sel_List(ref) = select;
    } else {
	IPtr     last = Sel_List(ref);

	while (Sel_Next(last) != Nil)
	    last = Sel_Next(last);
	Sel_Next(last) = select;
    }
}

/* (ref,x,y):ref ref(x..y), (val,x,y):val val(x..y) */
void    opSlice()
{
    TRI_ENTER;
    C_Ref   id;

    switch (Kind(R(First_Op))) {
      case Tuple:
      case String:
	id = Id_Watched(R(First_Op));
	break;
      default:
	id = 0;
    }

    if (id != 0) {
	show("! Evaluate: ", stdout);
	show(Id_String(id), stdout);
	show("(", stdout);
	mcPrint(R(Second_Op), stdout, verbose);
	show("..", stdout);
	mcPrint(R(Third_Op), stdout, verbose);
	show(")", stdout);
	mcFlush(stdout);
    }
    switch (Kind(R(First_Op))) {
      case Ref:{
	    IPtr     select = New_Block(Sel, 0);

	    At_Index(select) = R(Second_Op);
	    To_Index(select) = R(Third_Op);
	    Sel_Is(select) = Sel_Slice;
	    R(ans) = R(First_Op);
	    Add_Sel(select, R(ans));
	} break;

	/* Select the element from R(Second_Op) to R(Third_Op) */
      case String:
      case Tuple:{
	    Idx     orig = (Kind(R(First_Op)) == Tuple
			    ? Tuple_Origin(R(First_Op))
			    : 1);
	    Idx     at,
	            to;

	    if (R(Second_Op) == Nil)
		at = orig;
	    else if (!mpToInt(R(Second_Op), &at))
		BAD_SLICE(NULL, R(First_Op), R(Second_Op), R(Third_Op));

	    if (R(Third_Op) == Nil)
		to = mcLen(R(First_Op))+orig-1;
	    else if (!mpToInt(R(Third_Op), &to))
		BAD_SLICE(NULL, R(First_Op), R(Second_Op), R(Third_Op));

	    if (at < orig)
		BAD_SLICE(NULL, R(First_Op), R(Second_Op), R(Third_Op));

	    if (Kind(R(First_Op)) == Tuple) {
		R(ans) = mcTSlice(Tuple_Value(R(First_Op)), at-orig, to-orig);
		R(ans) = New_Tuple(R(ans), orig);
	    } else {		       /* Kind == String */
		R(ans) = mcSSlice(R(First_Op), at, to);
	    }
	    YIELDS;
	} break;

      default:
	BAD_SLICE(NULL, R(First_Op), R(Second_Op), R(Third_Op));
    }
    TRI_LEAVE;
}


/* (i,t): t with origin i */
void opOrig() {
    BIN_ENTER;
    IPtr i,t;
    switch( Pair(Kind(R(Left_Op)), Kind(R(Right_Op))) ) {
    case Pair(Integer, Tuple): i = R(Left_Op); t = R(Right_Op); break;
    case Pair(Tuple, Integer): t = R(Left_Op); i = R(Right_Op); break;
    default:
	BAD_ARGS("@", R(Left_Op), R(Right_Op));
    }
    R(ans) = New_Tuple( Tuple_Value(t), (Idx)Int_Value(i) );
    BIN_LEAVE;
}
