#include "common.h"
/* $Header: init_ops.c,v 3.2 90/04/25 16:22:48 gary Locked $ */
#include "instr.h"
#include "op.h"
#include "manager.h"
#include "micro.h"
#include "emitter.h"

#define Mk_Map(instr,code,name) \
	{ Inst_Map[instr].Action = code;\
	  Inst_Map[instr].Name	 = name;\
	}

struct Definition Inst_Map[(NoOp) + 1];

/* Must be called before any Operations are executed. */
void    Init_Operations()
{
    Mk_Map(Div, opDiv, "Div")
    Mk_Map(Eq, opEq, "Eq")
    Mk_Map(Exp, opExp, "Exp")
    Mk_Map(Ge, opGe, "Ge")
    Mk_Map(Gt, opGt, "Gt")
    Mk_Map(Impl, opImpl, "Impl")
    Mk_Map(In, opIn, "In")
    Mk_Map(Le, opLe, "Le")
    Mk_Map(Lt, opLt, "Lt")
    Mk_Map(Minus, opMinus, "Minus")
    Mk_Map(Mod, opMod, "Mod")
    Mk_Map(Ne, opNe, "Ne")
    Mk_Map(Negate, opNegate, "Negate")
    Mk_Map(Not, opNot, "Not")
    Mk_Map(Notin, opNotin, "Notin")
    Mk_Map(Plus, opPlus, "Plus")
    Mk_Map(And, opAnd, "And")
    Mk_Map(Or, opOr, "Or")
    Mk_Map(Slash, opSlash, "Slash")
    Mk_Map(Subset, opSubset, "Subset")
    Mk_Map(Times, opTimes, "Times")
    Mk_Map(With, opWith, "With")
    Mk_Map(Less, opLess, "Less")
    Mk_Map(Br, opBr, "Br")
    Mk_Map(Br_False, opBr_False, "Br_False")
    Mk_Map(Br_True, opBr_True, "Br_True")
    Mk_Map(Cont, opCont, "Cont")
    Mk_Map(Cnt2, opCnt2, "Cnt2")
    Mk_Map(Cnt3, opCnt3, "Cnt3")
    Mk_Map(Count, opCount, "Count")
    Mk_Map(Mk_Tuple, opMk_Tuple, "Mk_Tuple")
    Mk_Map(Mk_Set, opMk_Set, "Mk_Set")
    Mk_Map(Mk_Iter, opMk_Iter, "Mk_Iter")
    Mk_Map(Mk_FIter, opMk_FIter, "Mk_FIter")
    Mk_Map(Mk_MIter, opMk_MIter, "Mk_MIter")
    Mk_Map(Mk_2Cnt, opMk_2Cnt, "Mk_2Cnt")
    Mk_Map(Mk_3Cnt, opMk_3Cnt, "Mk_3Cnt")
    Mk_Map(Cl_Iter, opCl_Iter, "Cl_Iter")
    Mk_Map(Env_Extend, opEnv_Extend, "Env_Extend")
    Mk_Map(Env_Pop, opEnv_Pop, "Env_Pop")
    Mk_Map(Mk_Closure, opMk_Closure, "Mk_Closure")
    Mk_Map(Mk_Env, opMk_Env, "Mk_Env")
    Mk_Map(Read, opRead, "Read")
    Mk_Map(ReadF, opReadF, "ReadF")
    Mk_Map(Print, opPrint, "Print")
    Mk_Map(PrintF, opPrintF, "PrintF")
    Mk_Map(Cons, opCons, "Cons")
    Mk_Map(Pop, opPop, "Pop")
    Mk_Map(PNil, opPNil, "PNil")
    Mk_Map(PTrue, opPTrue, "PTrue")
    Mk_Map(PFalse, opPFalse, "PFalse")
    Mk_Map(POM, opPOM, "POM")
    Mk_Map(PNP, opPNP, "PNP")
    Mk_Map(PAtom, opPAtom, "PAtom")
    Mk_Map(Slice, opSlice, "Slice")
    Mk_Map(SMap, opSMap, "SMap")
    Mk_Map(MMap, opMMap, "MMap")
    Mk_Map(Return, opReturn, "Return")
    Mk_Map(PRef, opPRef, "PRef")
    Mk_Map(Assign, opAssign, "Assign")
    Mk_Map(From, opFrom, "From")
    Mk_Map(Fromb, opFromb, "Fromb")
    Mk_Map(Frome, opFrome, "Frome")
    Mk_Map(Load, opLoad, "Load")
    Mk_Map(Dup, opDup, "Dup")
    Mk_Map(Swap, opSwap, "Swap")
    Mk_Map(Iter_Next, opIter_Next, "Iter_Next")
    Mk_Map(Rev, opRev, "Rev")
    Mk_Map(Orig, opOrig, "Origin")
    Mk_Map(Iter_Load, opIter_Load, "Iter_Load")
    Mk_Map(LookUp, opLookUp, "LookUp")
    Mk_Map(NoOp, opNoOp, "NoOp")       /* Must be last to terminate list. */
}
