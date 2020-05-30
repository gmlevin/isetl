/* $Header: manager.h,v 3.1 90/04/25 16:24:57 gary Locked $ */

/* Balance factor for AVL-Trees */
#define max_delta  1
extern  Idx def_orig;		       /* default origin for tuples */

extern void Mark_All(PTR);
extern void Init_Store(NONE);	       /* (): Must be called before first
				        * New_Block */
extern IPtr New_Block(TAGS X INT);      /* (T,len): Returns block of type T
				        * for String, len is length of
				        * string */
extern IPtr New_Boolean(BOOL);	       /* (n): Return Boolean with value n */
extern IPtr New_Integer(PWORKING);      /* (n): Return Integer with value n */
extern IPtr New_File(C_FILE X CHAR);    /* (n,c): Return File with value n
				        * mode c */
extern IPtr New_Real(C_REAL);	       /* (n): Return Real with value n */
extern IPtr New_Rational(PTR X PTR);   /* (n,d): Return Rational n/d */
extern IPtr New_String(STR);	       /* (p): Return String with value
				        * strcpy(p) */
extern IPtr New_Tuple(PTR X IDX);       /* (x,i): Return tuple with values in
				        * x, origin i*/
extern IPtr New_Set(PTR);	       /* (x): Return set with values in x */
extern IPtr New_Tree(PTR X PTR);	       /* (left,right): tree  (assumes
				        * balance) */
extern IPtr New_HTree(PTR X PTR);       /* (left,right): Htree  (assumes
				        * balance) */
extern void HTFix(PTR);		       /* (HTree) assume subtrees
				        * balanced.  sets necessary fields */
extern void GC(INT);


/* EXTERNAL Ptrs */
/* These are the only Ptrs known to GC.	*/
/*   Environment, AStack[], Top, PC,	*/
/*   New_Code, Live_AST, Last_Line	*/

/* Non-garbage collectable constants */
/*    NilHT, NilT, Nil, OM, ITRUE, IFALSE,	*/
/*    BigOne, BigTwo, BigTen			*/
/*
 * Nil represents the empty list.
 *	Next(Nil)==Nil.
 *	Info(Nil)==OM.
 * OM represents the undefined value.
 * ITRUE and IFALSE are the ISETL Boolean values.
 * NilT and NilHT are Nil for trees and hashtrees.
 * BigX are Bignum constants
 */

/* Limit on number of distinct ids permitted */
#define Max_Vars	300


#define Var(ref)	Var_Value(Var_Assoc(ref))
extern long max_mem;
extern long current_mem;


