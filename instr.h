/* $Header: instr.h,v 3.2 90/04/25 16:24:51 gary Locked $ */
/*
 * This defines the instructions understood by the interpreter. Each
 * instruction must first appear in the list for the type Instruction.
 * Then a corresponding entry must be made in the array Inst_Map
 * (init_ops.c). NoOp must remain the last Instruction.
 */

typedef C_Instr Instruction;

/* enum Instruction { */
#define		Div		1
#define		Eq		2
#define		Exp		3
#define		Ge		4
#define		Gt		5

#define		Impl		6
#define		In		7
#define		Le		8
#define		Lt		9
#define		Minus		10

#define		Mod		11
#define		Ne		12
#define		Negate		13
#define		Not		14
#define		Notin		15

#define		Plus		16
#define		Slash		17
#define		Subset		18
#define		Times		19
#define		With		20
#define		Less		21

#define		And		22
#define		Or		23


#define		Br		24
#define		Br_False	25
#define		Br_True		26
#define		Cont		27


#define		Cnt2		28
#define		Cnt3		29
#define		Count		30

#define		Print		31
#define		PrintF		32
#define		Read		33
#define		ReadF		34

#define		Pop		35
#define		Cons		36
#define		Dup		37
#define		Swap		38

#define		PNil		40

#define		PTrue		43
#define		PFalse		44
#define		POM		45
#define		PAtom		46

#define		Mk_Tuple	47
#define		Mk_Set		48
#define		Mk_Iter		49
#define		Mk_FIter	50
#define		Mk_MIter	51
#define		Mk_2Cnt		52
#define		Mk_3Cnt		53

#define		Mk_Closure	54
#define		Mk_Env		55
#define		Cl_Iter		56
#define		Env_Extend	57
#define		Env_Pop		58

#define		Iter_Next	59

#define		Slice		60
#define		SMap		61
#define		MMap		62
#define		Return		63

#define		Assign		64
#define		PRef		65
#define		Load		66
#define		Frome		67
#define		Fromb		68
#define		From		69
#define		Rev		70
#define		Orig		71
#define		Iter_Load	72

#define         PNP             73
#define         LookUp          74
#define		NoOp		75
/* }; */

/* Must be called before any execution */
extern void Init_Operations(NONE);



/*
 * Procedures to perform type checking and selection of the lower level
 * operations (micro-code) for the ISETL interpreter.
 *
 * Arguments are on the stack.	Have no parameters.
 */

/*
 * Operations remove arguments from stack and replace them by result. In
 * the definitions, (a,b,c):T indicates a, b, and, c are arguments --with
 * a pushed first, c last-- and T is type returned.  In general,
 * operations are polymorphic and perform the ISETL operations;  in this
 * case, the type is *.  No type indicates no result.
 */
extern void opDiv(NONE);
extern void opEq(NONE);
extern void opExp(NONE);
extern void opGe(NONE);
extern void opGt(NONE);
extern void opImpl(NONE);
extern void opIn(NONE);
extern void opLe(NONE);
extern void opLt(NONE);
extern void opMinus(NONE);
extern void opMod(NONE);
extern void opNe(NONE);
extern void opNegate(NONE);
extern void opNot(NONE);
extern void opNotin(NONE);
extern void opPlus(NONE);
extern void opAnd(NONE);
extern void opOr(NONE);
extern void opSlash(NONE);
extern void opSubset(NONE);
extern void opTimes(NONE);
extern void opWith(NONE);
extern void opLess(NONE);

extern void opBr(NONE);		       /* (br) Branch to br		 */
extern void opBr_False(NONE);	       /* (pred,br) goto br if not pred */
extern void opBr_True(NONE);	       /* (pred,br) goto br if pred */
extern void opCont(NONE);	       /* Continue with next block */

extern void opCnt2(NONE);	       /* (low,high):List low..high	 */
extern void opCnt3(NONE);	       /* (1st,2nd,3rd): List 1st,2nd..3rd */
extern void opCount(NONE);	       /* (x): Int  Count of x */

extern void opPrintF(NONE);	       /* (x,f) Prints elements of list x
				        * on f, elements of x come in
				        * pairs (value,format). If f is
				        * not a file, gives error */
extern void opPrint(NONE);	       /* (x,f) Prints each element of
				        * list x on f, followed by \n.  If
				        * f is not a file, gives error */
extern void opRead(NONE);	       /* (x,f) Reads objects from f
				        * assigns them to each element of
				        * x.  If f is not a file, gives
				        * error */
extern void opReadF(NONE);	       /* (x,f) Reads objects from f
				        * assigns them to each element of
				        * x. If f is not a file, gives
				        * error */

extern void opPop(NONE);	       /* (x)	Ignores x.  Deletes it
				        * from Stack */
extern void opCons(NONE);	       /* (x,L):List cons(x,L) */
extern void opDup(NONE);	       /* (x):x,x  Duplicates the top of
				        * stack */
extern void opSwap(NONE);	       /* (x,y):y,x  Swaps top two stack
				        * elements */



extern void opMk_Tuple(NONE);	       /* (x):Tuple Coerces x to a Tuple */
extern void opMk_Set(NONE);	       /* (x):Set Coerces x to a Set */
extern void opMk_Iter(NONE);	       /* (PrevIter, LHS, S):Iter Creates
				        * an iterator (in) */
extern void opMk_FIter(NONE);	       /* (PrevIter, LHS, S):Iter Creates
				        * an iterator (SMAP) */
extern void opMk_MIter(NONE);	       /* (PrevIter, LHS, S):Iter Creates
				        * an iterator (MMAP) */
extern void opMk_2Cnt(NONE);	       /* (PrevIter, LHS, start,stop):Iter
				        * Creates iterator (in
				        * [start..stop] ) */
extern void opMk_3Cnt(NONE);	       /* (PrevIter, LHS,
				        * start,next,stop): Iter Creates
				        * iterator (in [start,next..stop]) */
extern void opCl_Iter(NONE);	       /* (iter) Removes iter and
				        * corresponding Assoc */
extern void opEnv_Extend(NONE);	       /* () Fetches var index and extends
				        * Env */
extern void opEnv_Pop(NONE);	       /* () Removes an assoc from Env */
extern void opMk_Closure(NONE);	       /* (code):Closure
				        * <code,Environment> */
extern void opMk_Env(NONE);	       /* (args) Fetches parms and locals;
				        * binds args-parms, om-locals on
				        * top of Environment */
extern void opIter_Next(NONE);	       /* (i):Iter Advances iter i,
				        * branches on done */

extern void opPInteger(NONE);	       /* ():Int Fetches n and pushes as
				        * Integer */
extern void opPNil(NONE);	       /* ():List Pushes Nil */
extern void opPString(NONE);	       /* ():String Fetches and pushes as
				        * String */
extern void opPReal(NONE);	       /* ():Real Fetches and pushes Real */
extern void opPTrue(NONE);	       /* ():Boolean Push True */
extern void opPFalse(NONE);	       /* ():Boolean Push False */
extern void opPOM(NONE);	       /* ():Undefined Push OM */
extern void opPNP(NONE);	       /* ():Undefined Push NP */
extern void opPAtom(NONE);	       /* ():Atom Push a new atom */
extern void opPRef(NONE);	       /* ():ref Fetches var index and
				        * pushes ref */

extern void opSMap(NONE);	       /* (ref,x):ref ref(x), (val,x):val
				        * val(x) */
extern void opMMap(NONE);	       /* (ref,x):ref ref{x}, (val,x):val
				        * val{x} */
extern void opSlice(NONE);	       /* (ref,x,y):ref ref(x..y),
				        * (val,x):val val(x..y) */
extern void opReturn(NONE);	       /* (return-closure,x): x Resets PC
				        * and Environment to
				        * return-closure */

extern void opAssign(NONE);	       /* (ref,val) ref := val */
extern void opFrom(NONE);	       /* (ref1,ref2) take ref1 from ref2 */
extern void opFromb(NONE);	       /* (ref1,ref2) take ref1 fromb ref2 */
extern void opFrome(NONE);	       /* (ref1,ref2) take ref1 frome ref2 */
extern void opLoad(NONE);	       /* (ref):*   De-reference top of
				        * stack */
extern void opRev(NONE);	       /* (list):list Reverses list */

extern void opOrig(NONE);	       /* (i,t): t with origin i */
extern void opIter_Load(NONE);	       /* (iter): * leaves values of iter */
extern void opLookUp(NONE);            /* (list of str and int): ref */
extern void opNoOp(NONE);	       /* ()	No operation */

struct Definition {
    void    (*Action) (NONE);	       /* procedure that performs the
				        * operation	 */
    char   *Name;		       /* String describing operation */
};

extern struct Definition Inst_Map[];
