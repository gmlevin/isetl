#include "common.h"
/* $Header: op_ctrl.c,v 3.1 90/04/25 16:23:43 gary Locked $ */
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


/* (br) Branch to br		 */
void    opBr()
{
    NO_UNARY_ENTER;
    assert(Kind(R(Only_Op)) == Label);
    GO_TO(Label_Value(R(Only_Op)));
    UNARY_NO_RESULT;
}

/* (pred, br) Branch to br if not pred */
void    opBr_False()
{
    NO_BIN_ENTER;
    assert(Kind(R(Right_Op)) == Label);

    if (Kind(R(Left_Op)) != Boolean) {
	BAD_VAL("Boolean expected", R(Left_Op));
    } else if (Bool_Value(R(Left_Op))) {
	/* Do nothing */
    } else {
	GO_TO(Label_Value(R(Right_Op)));
    }
    BIN_NO_RESULT;
}

/* (a) Branch to label following if a	 */
void    opBr_True()
{
    NO_BIN_ENTER;
    assert(Kind(R(Right_Op)) == Label);

    if (Kind(R(Left_Op)) != Boolean) {
	BAD_VAL("Boolean expected", R(Left_Op));
    } else if (Bool_Value(R(Left_Op))) {
	GO_TO(Label_Value(R(Right_Op)));
    } else {
	/* Do nothing */
    }
    BIN_NO_RESULT;
}

/* () Branch to next code block */
void    opCont()
{
    GO_TO(CNext(EP_Ptr(PC)));
}



/* (iter) Remove iter and corresponding Assoc in Environment */
void    opCl_Iter()
{
    NO_UNARY_ENTER;

    while (R(Only_Op) != Nil) {
	mcDel_Env(Iter_LHS(R(Only_Op)));
	R(Only_Op) = Iter_Rest(R(Only_Op));
    }
    UNARY_NO_RESULT;
}



/* (i,br):Iter Advances iter i, branches on done */
/* always leaves i on stack */
void    opIter_Next()
{
    /* IPtr	*br = Top;  accessed by opBr, not needed here */
    IPtr    *i = Top - 1;
    Bool    empty = false;
    Reg     (target);
    Reg     (expr);

    /* Push ref for LHS */
    R(target) = Iter_LHS(R(i));

    if (Kind(R(i)) != Iter)
	RT_ERROR("Iter_Next(internal error)");

    switch (Iter_Kind(R(i))) {
      case I_Cnt:{
	    IPtr    start, stop, step;

	    start = Info(Iter_Source(R(i)));
	    stop  = Info(Next(Iter_Source(R(i))));
	    step  = Info(Next(Next(Iter_Source(R(i)))));


	    /* Iter_Source(R(i)) = <start, stop, step> */
	    switch( Kind(Info(Iter_Source(R(i)))) ) {
	      case Integer: {
		C_Int   istart,
			istop,
			istep;

		istart = Int_Value(start);
		istop  = Int_Value(stop);
		istep  = Int_Value(step);

		if (istep == 0
		    || (istep > 0 ? istart > istop : istart < istop)) {
		    empty = true;
		} else {
		    R(expr) = start;
		    Info(Iter_Source(R(i)))
			= New_Integer((WORKING) istart + istep);
		}
	      } break;

	      case Bignum: {
		Reg     (temp);

		if (Length(step) == 0
		    || Sign(step) * mpCmp(start, stop) > 0) {
		    empty = true;
		} else {
		    R(expr) = mpNorm(start);
		    R(temp) = mpAdd(start, step);
		    R(temp) = mpForce(R(temp));
		    Info(Iter_Source(R(i))) = R(temp);
		}
		mcPop();	       /* temp */
	      } break;

	      case Rational: {
		Reg     (temp);

		if (Length(Num(step)) == 0
                    || (Sign(Num(step)) > 0 
                         ?  mcLt(stop,start) : mcLt(start,stop) ) ) {
		    empty = true;
		} else {
		    R(expr) = RatNorm(Num(start),Den(start));
		    R(temp) = RatAdd(start, step);
		    R(temp) = RatForce(R(temp));
		    Info(Iter_Source(R(i))) = R(temp);
		}
		mcPop();	       /* temp */
	      } break;

	      case Real: {    /* <start, number of terms, step> */
		C_Real  rstart,
			rstep;
		C_Int	istop;

		rstart = Real_Value(start);
		istop  = Int_Value(stop);
		rstep  = Real_Value(step);

		if (Iter_Cnt(R(i)) > istop) {
		    empty = true;
		} else {
		    R(expr) = New_Real(rstart + Iter_Cnt(R(i))*rstep);
		    Iter_Cnt(R(i))++;
		}
	      } break;

	      case String: {
		if ( Str_Value(start)[0] > Str_Value(stop)[0] ) {
		    empty = true;
		} else {
		    char text[2];
		    text[0] = Str_Value(start)[0]+1;
		    text[1] = 0;
		    R(expr) = start;
		    Info(Iter_Source(R(i))) = New_String(text);
		}
	      } break;
	    }
	} break;

      case M_Set:{
	    Reg     (indx);
	    Reg     (item);

	    if (!mcNext(&R(item), &Iter_Source(R(i)), true)) {
		empty = true;
	    } else {
		/* Choose an indx */
		if (Kind(R(item)) != Tuple || mcLen(R(item)) != 2)
		    RT_ERROR("Bad mmap in iterator");
		R(indx) = mcAccess(Tuple_Value(R(item)), (Idx) 0);

		/* expr := set of images including one in item */
		R(expr) = mcImages(Iter_Source(R(i)), R(indx));
		R(expr) = mcHWith(mcAccess(Tuple_Value(R(item)),
					   (Idx) 1), R(expr));
		R(expr) = New_Set(R(expr));

		/* Delete images and build tuple [indx, image set] */
		Iter_Source(R(i)) = mcDel_Image(Iter_Source(R(i)),
						R(indx));
		Iter_Source(R(i)) = mcStart(Iter_Source(R(i)));
		R(expr) = mcTwo_Tuple(R(indx), R(expr));

	    }
	    /* delete indx, item from stack */
	    mcPop();
	    mcPop();
	} break;

      case F_Tup:
      case F_Set:
      case In_Set:
      case In_Tup:{
	    /* expr := next value, Advance through values */
	    if (!mcNext(&R(expr), &Iter_Source(R(i)),
			Iter_Kind(R(i)) == F_Tup
			|| Iter_Kind(R(i)) == In_Tup)) {
		empty = true;
	    } else {
		Iter_Cnt(R(i))++;
		if (Iter_Kind(R(i)) == F_Tup) {
		    R(expr) = mcCons(R(expr), Nil);
		    R(expr) = mcCons(Nil, R(expr));
		    Info(R(expr)) = New_Integer((WORKING) Iter_Cnt(R(i)));
		}
	    }
	} break;

      case F_Str:
      case In_Str:{
	    if (Iter_Cnt(R(i)) >= strlen(Str_Value(Iter_Source(R(i))))) {
		empty = true;
	    } else {
		char    singleton[2];
		IPtr     source;

		singleton[1] = EOS;
		/* expr := next value, Advance */
		source = Iter_Source(R(i));
		singleton[0] = Str_Value(source)[Iter_Cnt(R(i))];
		R(expr) = New_String(singleton);
		Iter_Cnt(R(i))++;
		if (Iter_Kind(R(i)) == F_Str) {
		    Reg     (indx);

		    R(indx) = New_Integer((WORKING) Iter_Cnt(R(i)));
		    R(expr) = mcTwo_Tuple(R(indx), R(expr));

		    mcPop();	       /* delete indx */
		}
	    }

	} break;

      case M_Str:
	RT_ERROR("Can't mmap string");
      case M_Tup:
	RT_ERROR("Can't mmap tuple");
      default:
	assert(0);
    }
    if (empty) {
	/* delete expr, ref, iterator and assoc, branch */
	mcPop();
	mcPop();
	opBr();			       /* Prepare Branch */
	mcDel_Env(Iter_LHS(R(Top)));
	R(Top) = Iter_Rest(R(Top));
    } else {
	opAssign();
	mcPop();		       /* Ignore Branch */
    }
}




/* (PrevIter, LHS, S):Iter Creates an iterator */
void GIter(INT), IC2(NONE), IC3(NONE);  /* static forward declarations */

void    opMk_Iter() {    GIter(I_In);}
void    opMk_FIter(){    GIter(I_SMap);}
void    opMk_MIter(){    GIter(I_MMap);}
void    opMk_2Cnt() {    IC2();    GIter(I_Cnt);}
void    opMk_3Cnt() {    IC3();    GIter(I_Cnt);}

static void GIter(k)
    Iter_Types k;
{
    TRI_ENTER;
    R(ans) = New_Block(Iter, 0);
    Iter_LHS(R(ans)) = R(Second_Op);
    Iter_Rest(R(ans)) = R(First_Op);
    Iter_Cnt(R(ans)) = 0;
    switch (Kind(R(Third_Op))) {
      case Set:
	Iter_Kind(R(ans)) = (k == I_In ? In_Set : k == I_SMap ? F_Set : M_Set);
	Iter_Source(R(ans)) = mcStart(Set_Value(R(Third_Op)));
	break;
      case Tuple:
	Iter_Kind(R(ans)) = (k == I_In ? In_Tup : k == I_SMap ? F_Tup : M_Tup);
	Iter_Source(R(ans)) = mcStart(Tuple_Value(R(Third_Op)));
	Iter_Cnt(R(ans)) = Tuple_Origin(R(Third_Op))-1;
	break;
      case String:
	Iter_Kind(R(ans)) = (k == I_In ? In_Str : k == I_SMap ? F_Str : M_Str);
	Iter_Source(R(ans)) = R(Third_Op);
	break;
      case List:
	Iter_Kind(R(ans)) = k;
	Iter_Source(R(ans)) = R(Third_Op);
	break;
      default:
	BAD_VAL("Can't iterate over", R(Third_Op));
    }
    TRI_LEAVE;
}

/* Create list of form <start, stop, step> */
static void IC2()
{
    BIN_ENTER;

    switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
      case Pair(Integer, Integer):
	R(ans) = New_Integer((WORKING) 1);
	break;
      case Pair(Integer, Bignum):
      case Pair(Bignum, Integer):
      case Pair(Bignum, Bignum):
	R(Left_Op) = mpForce(R(Left_Op));
	R(Right_Op) = mpForce(R(Right_Op));
	R(ans) = BigOne;
	break;
      case Pair(String,String):
	if( mcLen(R(Left_Op)) != 1 || mcLen(R(Right_Op)) != 1 ) {
	    RT_ERROR("String range over single chars only");
	}
	R(ans) = Nil;
	break;
      default:
	RT_ERROR("Bad args in low..high");
    }
    R(ans) = mcCons(R(ans), Nil);
    R(ans) = mcCons(R(Right_Op), R(ans));
    R(ans) = mcCons(R(Left_Op), R(ans));
    BIN_LEAVE;
}

static void IC3()
{
    TRI_ENTER;
    if (Kind(R(First_Op)) == Integer
	&& Kind(R(Second_Op)) == Integer
	&& Kind(R(Third_Op)) == Integer) {

	R(ans) = New_Integer((WORKING) Int_Value(R(Second_Op))
			     - Int_Value(R(First_Op)));
    } else if (
	is_big(R(First_Op)) && is_big(R(Second_Op)) && is_big(R(Third_Op))
	) {
	R(First_Op) = mpForce(R(First_Op));
	R(Second_Op) = mpForce(R(Second_Op));
	R(Third_Op) = mpForce(R(Third_Op));
	R(ans) = mpSub(R(Second_Op), R(First_Op));
	R(ans) = mpForce(R(ans));
    } else if (
	is_rat(R(First_Op)) && is_rat(R(Second_Op)) && is_rat(R(Third_Op))
	) {
	R(First_Op) = RatForce(R(First_Op));
	R(Second_Op) = RatForce(R(Second_Op));
	R(Third_Op) = RatForce(R(Third_Op));
	R(ans) = RatSub(R(Second_Op), R(First_Op));
	R(ans) = RatForce(R(ans));
    } else if (
	is_num(R(First_Op)) && is_num(R(Second_Op)) && is_num(R(Third_Op))
	) {  /* Form <start, cnt, step> */
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

	R(ans) = New_Real(step);
	R(Third_Op) = New_Integer((WORKING)cnt);

    } else {
	RT_ERROR("Bad args in low,next..high");
    }

    R(ans) = mcCons(R(ans), Nil);
    R(ans) = mcCons(R(Third_Op), R(ans));
    R(ans) = mcCons(R(First_Op), R(ans));

    TRI_LEAVE;
}


/* Implement ISETL func */

static int func_cnt = 0;
/* (code,doc):Closure <code,Environment,doc> */
void    opMk_Closure()
{
    BIN_ENTER;
    assert(Kind(R(Left_Op)) == Label);
    func_cnt++;

    R(ans) = New_Block(Closure, 0);
    Cl_Code(R(ans)) = Label_Value(R(Left_Op));
    assert(Kind(Cl_Code(R(ans))) == Join);
    Cl_Env(R(ans)) = Environment;
    Cl_SRC(R(ans)) = R(Right_Op);
    Cl_Cnt(R(ans)) = func_cnt;
    BIN_LEAVE;
}


/*
 * (args) Fetches parms and locals; binds args-parms, om-locals on top of
 * Environment
 *
 * args is a list of arguments
 */
void    opMk_Env()
{
    C_Ref   var;

    NO_UNARY_ENTER;

    /* for each parameter, add an Assoc to Environment and init */
    var = Get_Ref;
    while (var != 0 && R(Only_Op) != Nil) {
	TRACE_VARS("Parameter", var);
	Environment = mcCons(Nil, Environment);
	Info(Environment) = New_Block(Assoc, 0);
	Var_Index(Info(Environment)) = var;
	Var_Value(Info(Environment)) = Info(R(Only_Op));

	if (Watched(var)) {
	    char    temp[100];

	    sprintf(temp, "! parameter %s := ", Id_String(var));
	    show(temp, stdout);
	    dbPrint(Info(R(Only_Op)), stdout);
	    mcFlush(stdout);
	}
	var = Get_Ref;
	R(Only_Op) = Next(R(Only_Op));
    }
    if (var != 0)
	RT_ERROR("Too few arguments");

    /* for each opt, add an Assoc to Environment and init */
    var = Get_Ref;
    while (var != 0) {
	TRACE_VARS("Opt", var);
	Environment = mcCons(Nil, Environment);
	Info(Environment) = New_Block(Assoc, 0);
	Var_Index(Info(Environment)) = var;
	Var_Value(Info(Environment)) = Info(R(Only_Op));

	if (Watched(var)) {
	    char    temp[100];

	    sprintf(temp, "! parameter %s := ", Id_String(var));
	    show(temp, stdout);
	    dbPrint(Info(R(Only_Op)), stdout);
	    mcFlush(stdout);
	}
	var = Get_Ref;
	R(Only_Op) = Next(R(Only_Op));
    }
    if (R(Only_Op) != Nil)
	RT_ERROR("Too many arguments");

    /* for each local, add an Assoc to Environment */
    for (var = Get_Ref; var != 0; var = Get_Ref) {
	TRACE_VARS("Local", var);
	Environment = mcCons(Nil, Environment);
	Info(Environment) = New_Block(Assoc, 0);
	Var_Index(Info(Environment)) = var;
    }

    UNARY_NO_RESULT;
}

/*
 * (return-closure,x): x Resets PC and Environment to return-closure Needs
 * to manipulate stack, so non-standard ENTER-LEAVE is used. None of the
 * operations creates space, so that it is safe to have the values x and
 * ret_closure not on the stack.
 *
 * mcPrint uses space, so need to be careful about x and ret_closure
 */
void    opReturn()
{
    IPtr     x,
            ret_closure;
    Bool    is_watched;

    call_level --;
    x = mcTopPop();

    /*
     * remove iterators that may be nested (return inside of
     * for-loop)
     */
    while (!mcIsEmpty() && Kind(R(Top)) == Iter) {
	mcPop();
    }

    ret_closure = R(Top);
    if (Kind(ret_closure) != Closure) {
	RT_ERROR("Return at top level");
    }
    is_watched = (Id_Watched(ret_closure) != 0);

    R(Top) = x;
    GO_TO(Cl_Code(ret_closure));
    Environment = Cl_Env(ret_closure);
    Last_Line   = Cl_SRC(ret_closure);


    if (is_watched) {
	ENTER;
	Reg     (rret);

	R(rret) = ret_closure;

	show("! ", stdout);
	show(Id_String(Id_Watched(ret_closure)), stdout);
	show(" returns: ", stdout);
	mcPrint(x, stdout, verbose);
	mcFlush(stdout);
	LEAVE;
    }
}

