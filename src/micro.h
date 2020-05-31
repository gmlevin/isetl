/* $Header: micro.h,v 3.2 90/04/25 16:24:59 gary Locked $ */
/*
 * Microcode routines manipulate internal representations.
 *
 * The result of microcode routines should not change Stack. (The values of
 * elements on the stack may change due to side-effects).
 *
 * IPtr parameters to microcode routines must be ACTIVE. The result from a
 * microcode routine *may be* INACTIVE, and hence may be dangerous.
 *
 *  Protocol for microcode routines is:
 *	Remember current stack.
 *	Add *C locals* as registers to stack.
 *	Compute using these registers (which are all active).
 *	Restore stack (which makes registers inactive).
 *	Return value (which may be in a register).
 *
 *
 * The last two steps are dangerous, in that an inactive value may be accessed
 * and returned.  However, as long as restoring the stack does not cause
 * allocation, the inactive registers will not be disturbed.
 *
 * To assist in this protocol, the following definitions are made. ENTER
 * Remember Stack for later restoring
 *
 *	Reg(x)		Create a register.
 *			Those created before ENTER will be left on the
 *			stack as values returned.
 *
 *	R(x)		The register x.	 Both lvalue and rvalue.
 *
 * 	LEAVE [expr]	Restores stack to state before ENTER.  If
 *			followed by expr, expr is returned.
 *
 * These definitions are also used by Operations. In Operations, Stack may
 * change.  Therefore, some Registers are created BEFORE ENTER, and remain
 * after LEAVE.
 */




#define ENTER		IPtr *Old_Top	= Top
#define Reg(x)		IPtr *x		= (mcPush(OM),Top)
#define R(x)		(*(x))
#define LEAVE		Top = Old_Top; return


extern void mcPLabel(C_LABEL);	       /* (): Push a label constant */

extern IPtr mcPdCall(PTR X PTR);

extern void show(STR X C_FILE);
extern void mcPrintF(PTR X PTR X C_FILE);	/* (x,form,f): Print
						 * x:form on f */


extern void mcPrint(PTR X C_FILE X BOOL);	/* (x,f,b): Print x on f
						 * (b<=>verbose) */
extern void mcFlush(C_FILE);	       /* (f): Flush output of mcPrint */
extern IPtr mcCnt(C_INT X C_INT X C_INT);	/* (i,j,step): return list
						 * i..j by step */
extern IPtr mcTrim(PTR);		       /* (x): Return x with trailing OMs
				        * deleted */
extern IPtr mcRev(PTR);		       /* (x): Return x in reverse order  */
extern IPtr mcCons(PTR X PTR);	       /* (x,L):List cons(x,L) */


extern IPtr mcStr_Concat(PTR X PTR);    /* (a,b:String):String Concat a and
				        * b */
extern IPtr mcStr_Rep(C_INT X PTR);     /* (n:int,S:string):string n
				        * repetitions of S */

extern IPtr mcUnion(PTR X PTR);	       /* (x,y:List):List Elem in x or y */
extern IPtr mcInter(PTR X PTR);	       /* (x,y:List):List Elem in both x
				        * and y */
extern IPtr mcDiff(PTR X PTR);	       /* (x,y:List):List Elem in x and
				        * not y */
extern IPtr mcRep(C_INT X PTR);	       /* (n:int,L:Tree):Tree n
				        * repetitions of L */


/* Predicates (return C Booleans) */
extern Bool mcIn_List(PTR X PTR);      /* (x:*,L:List):bool x in? L */
extern Bool mcIn_Tree(PTR X PTR);      /* (x:*,T:Tree):bool x in? T */
extern Bool mcEq(PTR X PTR);	       /* (x,y):bool x==y */
extern Bool mcLt(PTR X PTR);	       /* (x,y):bool x<y */
extern Bool mcSubset(PTR X PTR);       /* (x,y:List):bool x subset of y */
extern Bool mcSubstr(STR X STR);       /* (x,y:str):bool x substr of y */



/* To iterate through lists and trees */
extern Bool mcNext(PTR ADDR X PTR ADDR X BOOL);
extern IPtr mcStart(PTR);


extern Idx mcLen(PTR);		       /* (x):C_Int Length of x */

#define mcTLen(p)		( Kind(p)==Tree ? Len(p) : (Idx)1 )
#define mcHgt(p)		( Kind(p)==Tree ? Hgt(p) : 1 )

/* Return size of set representation */
#define mcHTLen(p)		( Kind(p)==HTree ? HLen(p) :  \
				  Kind(p)==List  ? mcLen(p) : 1 )
#define mcHHgt(p)		( Kind(p)==HTree ? HHgt(p) : 1 )
#define mcHMax(p)		( Kind(p)==HTree ? HMax(p) : mcHash(p,true) )

extern IPtr mcHAny(PTR);	       /* Random element from a hash tree */
extern int mcHash(PTR X BOOL);	       /* Hash value for an ISETL quantity
				        * is_element if hashing element of
				        * set */

extern IPtr mcList_Tree(PTR);	       /* (x): Return tuple with values in
				        * x */

extern IPtr mcVal(PTR);		       /* (ref):* de-ref */
extern void mcAssign(PTR X PTR);       /* (lhs,exp)  lhs:= exp */
extern void mcFrom(PTR X PTR);	       /* (lhs1,lhs2) take lhs1 from lhs2 */
extern void mcFromb(PTR X PTR);	       /* (lhs1,lhs2) take lhs1 fromb lhs2 */
extern void mcFrome(PTR X PTR);	       /* (lhs1,lhs2) take lhs1 frome lhs2 */

extern IPtr mcModify(PTR X PTR X PTR);  /* (val, sels, new_val):IPtr  ---
				        * val modified at sels to be
				        * new_val */
extern IPtr mcTSlice(PTR X IDX X IDX);  /* (tree,at,to):tree */
extern IPtr mcSSlice(PTR X IDX X IDX);  /* (str,at,to):str str(at..to) */
extern IPtr mcCat(PTR X PTR);	       /* (tree,tree):tree */
extern IPtr mcAccess(PTR X IDX);       /* (tree,int):* tree[int] */
extern IPtr mcUpdate(PTR X IDX X PTR); /* (tree,int,x):* tree[int] := x */
extern IPtr mcTwo_Tuple(PTR X PTR);    /* (x,y:IPtr):Tuple [x,y] */
extern IPtr mcDel_Image(PTR X PTR);    /* (f:list, i):list f-{[i,.]} */
extern IPtr mcWith(PTR X PTR);	       /* (i, S:set):set	S+{i} */
extern IPtr mcLess(PTR X PTR);	       /* (i, S:set):set	S-{i} */
extern IPtr mcLLess(PTR X PTR);	       /* (i, L:list):list      S-{i} */
extern IPtr mcImages(PTR X PTR);       /* (f:list, i):list	
				        * {y:y=f(x) | x=i} */
extern void mcDel_Env(PTR);	       /* (lhs) Deletes Assoc matching lhs */

#define mcPop()         (Top--)
#define mcTopPop()      (R(Top--))

#ifndef mcTopPop
#define mcPop           mcTopPop
extern IPtr mcTopPop(NONE);	       /* Pop stack and return old top
				        * value */
#endif

#define mcPush(x)	(R(++Top) = (x))
extern Bool mcIsEmpty(NONE);	       /* () Is stack empty? */
extern void mcMkEmpty(NONE);	       /* () Make stack empty */

extern IPtr mcHCat(PTR X PTR);
extern Bool mcIn_HTree(PTR X PTR);
extern IPtr mcHWith(PTR X PTR);
extern IPtr mcHLess(PTR X PTR);

/*
 * This procedure MODIFIES HTrees. Only use when you are certain there is
 * No Sharing.
 */
extern void mcNSHWith(PTR X PTR ADDR);



#define Exec(instr)	\
	( (*(Inst_Map[(int)instr] . Action)) () )


/* logically associated with microcode, appears in interpreter.c */
/* return the last char read, but not used  */
extern int mcRead(PTR X C_FILE X INT); /* (x,f): x := next object in f */
extern int mcReadF(PTR X PTR X C_FILE X INT);	/* (x,format,f): x := next
						 * object in f */



extern double rrand(NONE);


IPtr RatAdd(PTR X PTR);
IPtr RatSub(PTR X PTR);
IPtr RatTimes(PTR X PTR);
IPtr RatDiv(PTR X PTR);
IPtr RatForce(PTR);
IPtr RatCnt(PTR X PTR X PTR);
IPtr RatNorm(PTR X PTR);

