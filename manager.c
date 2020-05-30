#include "common.h"
/* $Header: manager.c,v 3.1 90/04/25 16:23:00 gary Locked $ */
#include "manager.h"
#include "micro.h"
#include "ast.h"
#include "parser.h"


#ifdef TORTURE
Bool    torture = false;	       /* Garbage collect before EVERY
				        * New_Block */
#endif

static long wanted=0, got=0;

/*
 * Store is allocated in segments, which are then linked together.
 *
 * Blocks are of varying length. Assumes that if all blocks are multiples of
 * Gen_Mult, then there will be no alignment problems.
 */
#define Round_Gen(x)    ((x) + (Gen_Mult-1) ) & (~(Gen_Mult-1))
#define Space_For(x)    (Round_Gen(sizeof(struct x)))





#define BYTES		10000

/* May allocate until memory is full */
/* Saves PREALLOC bytes for overhead */
#define PREALLOC	10000
#define INIT_SEGS	5

/* Never leave a segment smaller than MIN_FREE */
#define MIN_FREE	sizeof(struct Mini_Block)

typedef struct Segment *Seg_Ptr;
struct Segment {
    Seg_Ptr Next_Seg;
    C_Real  seg_dummy;		       /* Forces alignment */
    char    List_Space[BYTES];
};



static IPtr Free_List,
        Free_Tail;
static Seg_Ptr Store;
Idx    def_orig = 1;



/* Add a new Segment to Store and add the free space to Free_List */
Bool    Add_Segment()
{
    Seg_Ptr new;
    IPtr     p;

    GC_DEBUG("Added new segment\n",0,0);

    if ((sizeof(*new) + current_mem) > max_mem) {
	return false;
    }
    new = (Seg_Ptr) malloc(sizeof(*new));

    if (new == NULL)
	return false;

    current_mem = current_mem + sizeof(*new);

    new->Next_Seg = Store;
    Store = new;

    p = (IPtr) (new->List_Space);
    Next_Free(p) = Free_List;
    Kind(p) = Free;
    Mark(p) = 0;
    Size(p) = BYTES;
    if (Free_List == Nil)
	Free_Tail = p;
    Free_List = p;

    return true;
}


/* Initialize Store and Free_List */
void    Init_Store()
{

#ifdef TURBOC
    {
	extern void ctrlbrk(int);

	ctrlbrk(1);
    }
#endif

    Kind(Nil) = List;
    Next(Nil) = Nil;
    Info(Nil) = OM;

    Kind(NonPrint) = Undefined;

    Kind(OM) = Undefined;

    Kind(ITRUE) = Boolean;
    Bool_Value(ITRUE) = true;

    Kind(IFALSE) = Boolean;
    Bool_Value(IFALSE) = false;

    Kind(NilT) = Tree;
    Hgt(NilT) = 0;
    Len(NilT) = 0;
    Left(NilT) = OM;
    Right(NilT) = OM;

    Kind(NilHT) = HTree;
    HHgt(NilHT) = 0;
    HLen(NilHT) = 0;
    HMax(NilHT) = 0;
    HLeft(NilHT) = OM;
    HRight(NilHT) = OM;

    Kind(BigOne) = Bignum;
    Length(BigOne) = 1;
    Sign(BigOne) = 1;
    Digits(BigOne)[0] = 1;

    Kind(BigTwo) = Bignum;
    Length(BigTwo) = 1;
    Sign(BigTwo) = 1;
    Digits(BigTwo)[0] = 2;

    Kind(BigTen) = Bignum;
    Length(BigTen) = 1;
    Sign(BigTen) = 1;
    Digits(BigTen)[0] = 10;

#if 0
    if (gc_debug) {
	mcprintf("Sizes:\tMin Block =\t%5d\n", MIN_FREE,0);
	mcprintf("	  \tInt Block =\t%5d\n", sizeof(struct Int_Block),0);
	mcprintf("	  \tReal Block =\t%5d\n", sizeof(struct Real_Block),0);
	mcprintf("	  \tList Block =\t%5d\n", sizeof(struct List_Block),0);
	mcprintf("	  \tFree Block =\t%5d\n", sizeof(struct Free_Block),0);
	mcprintf("	  \tHead Block =\t%5d\n", sizeof(struct Block_Head),0);
	mcprintf("	  \tGen_Mult =\t%5d\n", Gen_Mult,0);
    }
#endif

    Store = 0;
    Free_List = Nil;
    Environment = Nil;
    Top = &AStack[0];
    New_Code = Nil;
    Source_List = ANULL;
    Last_Line = Nil;

    {
	int     i;
	char   *p = malloc(PREALLOC);

	for (i = 0; i < INIT_SEGS; i++) {
	    Add_Segment();
	}
	free(p);
    }

}




/*
 * New_Block(tag,len) ==> Block from free storage Kind := tag;	 Mark :=
 * 0; Size := actual size of block; for String, len = number of chars in
 * string
 */

static  next_atom = 1;
IPtr     New_Block(tag, len)
    Type_Tags tag;
    int     len;
{
    IPtr     new,
            source,
            pre;
    int     needed;

#ifdef Mac
    {
	static  count = 0;

	/*
	 * Perform a system call every so often to give apple-. a chance
	 * to interrupt.
	 */
	if (count++ > 1000) {
	    count = 0;
		quick_check();
	}
    }
#endif
#ifdef TURBOC
    {
	extern int kbhit(void);
	static  count = 0;

	/*
	 * Perform a system call every so often to give ^C a chance
	 * to interrupt.
	 */
	if (count++ > 300) {
	    count = 0;
	    kbhit();
	}
    }
#endif

    /* determine how many bytes are needed */
    switch (tag) {
      case SArray: case TArray:
	needed = Space_For(Array_Block)
	    + Round_Gen(len * sizeof(IPtr));
	break;
      case Atom:
	needed = Space_For(Atom_Block);
	break;
      case Boolean:
	needed = Space_For(Bool_Block);
	break;
      case Integer:
	needed = Space_For(Int_Block);
	break;
      case Bignum:
	needed = Space_For(BN_Block)
	    + Round_Gen(len * sizeof(DIGIT));
	break;
      case File:
	needed = Space_For(File_Block);
	break;
      case Real:
	needed = Space_For(Real_Block);
	break;
      case Rational:
	needed = Space_For(Rational_Block);
	break;
      case Set:
	needed = Space_For(Set_Block);
	break;
      case String:
	needed = Space_For(Str_Block) + Round_Gen(len + 1);
	break;
      case Tuple:
	needed = Space_For(Tuple_Block);
	break;
      case Label:
	needed = Space_For(Lab_Block);
	break;
      case Iter:
	needed = Space_For(Iter_Block);
	break;
      case Ref:
	needed = Space_For(Ref_Block);
	break;
      case Sel:
	needed = Space_For(Sel_Block);
	break;
      case List:
	needed = Space_For(List_Block);
	break;
      case Tree:
	needed = Space_For(Tree_Block);
	break;
      case HTree:
	needed = Space_For(HTree_Block);
	break;
      case AST:
	needed = Space_For(AST_Block);
	break;
      case Join:
	needed = Space_For(J_Block);
	break;
      case Code:
	needed = Space_For(F_Block)
	    + Gen_Mult + Round_Gen(len);
	break;
      case SRC:
      case PUSH:
      case Fork:
	needed = Space_For(F_Block);
	break;
      case Assoc:
	needed = Space_For(Assoc_Block);
	break;
      case Closure:
	needed = Space_For(Close_Block);
	break;
      case Predef:
	needed = Space_For(Pd_Block);
	break;
      default:
	assert(0);
    }

    wanted += needed;

    /* Add minimal overhead and round to Gen_Mult */
    if (needed < Space_For(Free_Block)) {
	needed = Space_For(Free_Block);
    }
    needed += Space_For(Block_Head);

    /* new := block with at least needed space */

#ifdef TORTURE
    {
	if (torture)
	    GC(0);
    }
#endif

    source = Nil;
    do {
	/* source := first block big enough to take needed space */
	source = Free_List;
	pre = Nil;
	while (source != Nil && Size(source) < needed) {
	    pre = source;
	    source = Next_Free(source);
	}

	if (source == Nil)
	    GC(needed);
    } while (source == Nil);

    /* cut new from source */
    if (Size(source) > needed + MIN_FREE) {
	/* cut new from end of source */
	new = (IPtr) ((char *) source + Size(source) - needed);
	Size(new) = needed;
	Size(source) -= needed;
    } else {
	/* use all of source and delete source from Free_List */
	new = source;
	if (pre == Nil)
	    Free_List = Next_Free(source);
	else
	    Next_Free(pre) = Next_Free(source);
	if (Next_Free(source) == Nil) {
	    Free_Tail = pre;
	}
    }

    got += Size(new);

    /* make Free_List begin where search ended */
    if (pre != Nil) {
	Next_Free(Free_Tail) = Free_List;
	Free_Tail = pre;
	Free_List = Next_Free(pre);
	Next_Free(pre) = Nil;
    }
    /* Initialize required fields */
    Kind(new) = tag;
    Mark(new) = 0;
    switch (tag) {
      case TArray: case SArray: {
	  int i;
	  Array_Len(new) = len;
	  for( i=0; i<len; i++ ) Array_Data(new)[i] = OM;
      } break;
      case Atom:
	Atom_Value(new) = (next_atom++);
	break;

      case Boolean:
      case Integer:
      case Bignum:
      case Real:
      case Undefined:
	break;			       /* No subfields are Ptrs */

      case Rational:
	Num(new) = OM;
	Den(new) = OM;
	break;

      case File:
	File_Value(new) = FNULL;
	File_Eof(new) = false;
	File_Last(new) = EOS;
	break;

      case SRC:
      case PUSH:
      case Fork:
	CPtr(new) = Nil;
	/* Fall through to rest of code blocks */
      case Join:
      case Code:
	CNext(new) = Nil;
	break;

      case Assoc:
	Var_Value(new) = OM;
	break;

      case Predef:
	Id_Watched(new) = 0;
	break;

      case Closure:
	Id_Watched(new) = 0;
	Cl_Env(new) = Nil;
	Cl_Code(new) = Nil;
	Cl_Override(new) = Nil;
	Cl_SRC(new) = Nil;
	break;

      case Ref:
	Var_Assoc(new) = Nil;
	Sel_List(new) = Nil;
	break;
      case Sel:
	At_Index(new) = OM;
	To_Index(new) = OM;
	Sel_Next(new) = Nil;
	break;

      case Set:
	Set_Value(new) = Nil;
	Set_Hash(new) = 0;
	Id_Watched(new) = 0;
	break;

      case String:
	Id_Watched(new) = 0;
	Str_Value(new)[0] = EOS;
	break;

      case Tuple:
	Id_Watched(new) = 0;
	Tuple_Value(new) = Nil;
	Tuple_Hash(new) = 0;
	break;

      case Label:
	Label_Value(new) = Nil;
	break;
      case Iter:
	Iter_LHS(new) = OM;
	Iter_Source(new) = OM;
	Iter_Rest(new) = Nil;
	break;

      case List:
	Info(new) = Nil;
	Next(new) = Nil;
	break;

      case Tree:
	Kind(new) = Tree;
	Hgt(new) = 0;
	Len(new) = 0;
	Left(new) = OM;
	Right(new) = OM;
	break;

      case HTree:
	Kind(new) = HTree;
	HHgt(new) = 0;
	HLen(new) = 0;
	HMax(new) = 0;
	HLeft(new) = OM;
	HRight(new) = OM;
	break;

      case AST:
	Kind(new) = AST;
	ast_1(new) = OM;
	ast_2(new) = OM;
	ast_kind(new) = -1;
	ast_more(new) = Live_AST;
	Live_AST = new;
	break;

      case Free:
	assert(0);
      default:
	assert(0);
    }

    return new;
}




/*
 * Mark_All( IPtr ) Mark all blocks reachable with Mark = 1
 */
void    Mark_All(p)
    IPtr     p;
{
start:if (p != Nil && Mark(p) == 0) {
	Mark(p) = 1;
	switch (Kind(p)) {
	  case TArray:  case SArray: {
	      int i;
	      for( i=0; i<Array_Len(p); i++ ) Mark_All(Array_Data(p)[i]);
	  } break;
	    
	  case Atom:
	  case Boolean:
	  case Integer:
	  case Bignum:
	  case Real:
	  case Undefined:
	  case Predef:
	  case File:
	  case String:
	    break;		       /* No subfields are Ptrs */

	  case Rational:
	    Mark_All(Num(p));
	    Mark_All(Den(p));
	    break;

	  case Fork:
	  case SRC:
	  case PUSH:
	  case Code:
	  case Join:
	    while (p != Nil) {
		Mark(p) = 1;
		if (Kind(p) == Fork || Kind(p) == PUSH
		    || Kind(p) == SRC)
		    Mark_All(CPtr(p));
		p = CNext(p);
	    }
	    break;

	  case Assoc:
	    p = Var_Value(p);	       /* Mark_All( Var_Value(p) ); */
	    goto start;
	    break;

	  case Closure:
	    Mark_All(Cl_Code(p));
	    Mark_All(Cl_SRC(p));
	    Mark_All(Cl_Override(p));
	    p = Cl_Env(p);	       /* Mark_All( Cl_Env(p) ); */
	    goto start;
	    break;

	  case Ref:
	    Mark_All(Var_Assoc(p));
	    Mark_All(Sel_List(p));
	    break;

	  case Set:
	    Mark_All(Set_Value(p));
	    break;

	  case Tuple:
	    Mark_All(Tuple_Value(p));
	    break;

	  case Label:
	    Mark_All(Label_Value(p));
	    break;

	  case Iter:
	    Mark_All(Iter_LHS(p));
	    Mark_All(Iter_Source(p));
	    Mark_All(Iter_Rest(p));
	    break;

	  case Sel:
	    while (p != Nil) {
		Mark(p) = 1;
		Mark_All(At_Index(p));
		Mark_All(To_Index(p));
		p = Sel_Next(p);
	    }
	    break;

	  case List:
	    while (p != Nil) {
		Mark(p) = 1;
		Mark_All(Info(p));
		p = Next(p);
	    }
	    break;

	  case Tree:
	    Mark(p) = 1;
	    Mark_All(Left(p));
	    Mark_All(Right(p));
	    break;

	  case HTree:
	    Mark(p) = 1;
	    Mark_All(HLeft(p));
	    Mark_All(HRight(p));
	    break;

	  case AST:
	    while (p != ANULL) {
		Mark(p) = 1;
		if (ast_kind(p) == T_Source)
		    Mark_All(ast_1(p));
		p = ast_more(p);
	    }
	    break;

	  case Free:
	    assert(0);
	  default:
	    assert(0);
	}
    }
}




/*
 * GC() Traverse all storage, linking up free blocks and setting all Mark
 * = 0.
 *
 * May get more space if needed. (If less than one BYTESworth reclaimed.)
 */
void    GC(needed)
    int     needed;
{
    Seg_Ptr seg;
    long    reclaimed,
            inuse,
            biggest,
            unmarked,
            marked;

#ifdef Signals
    Hold_Signals();
#endif

    /* Call Mark_All on all EXTERNAL pointers.  */
    Mark_All(Environment);
    Mark_Globals();
    {
	IPtr    *p;

	for (p = Top; p > AStack; p--) {
	    Mark_All(*p);
	}
    }
    Mark_All(EP_Ptr(PC));
    Mark_All(New_Code);
    Mark_All(Live_AST);
    Mark_All(Last_Line);
    Mark_All(Func_List);

    reclaimed = inuse = biggest = unmarked = marked = 0;

    /* Check all nodes in all segments */
    Free_List = Nil;
    for (seg = Store; seg != 0; seg = seg->Next_Seg) {
	char   *p;

	for (p = seg->List_Space;
	     p < (seg->List_Space) + BYTES;
	     p = p + Size((IPtr) p)) {

	    if (Mark((IPtr) p) == 1) {
		Mark((IPtr) p) = 0;
		marked++;
		inuse += Size((IPtr) p);
	    } else {
		unmarked++;
		reclaimed += Size((IPtr) p);

		/* if reclaiming a File */
		{
		    FILE * f;
		    if (Kind((IPtr) p) == File
			&& (f=File_Value((IPtr) p)) != FNULL
			&& f != stdin && f != stdout )
			fclose(f);
		}

		/* if reclaiming an AST */
		if (Kind((IPtr) p) == AST) {
		    switch (ast_kind((IPtr) p)) {
		      case T_Id:
		      case T_Integer:
		      case T_Real:
		      case T_String:
			free((char *) (ast_1((IPtr) p)));
		    }
		}
		Kind((IPtr) p) = Free;

		if ((char *) Free_List + Size(Free_List) == p) {
		    /* coalese */
		    Size(Free_List) += Size((IPtr) p);
		} else {
		    /* Insert */
		    Next_Free((IPtr) p) = Free_List;
		    if (Free_List == Nil) {
			Free_Tail = (IPtr) p;
		    }
		    Free_List = (IPtr) p;
		}

		if (Size(Free_List) > biggest)
		    biggest = Size(Free_List);
	    }
	}
    }

#ifdef TURBOC
    {   if( !direct_input
#ifdef GRAPHICS
                && !Graphics
#endif
               ) {
	    if( !still_running ) {
		popup(30,8,"Running");
		cprintf("%s", "ISETL is still running.\r\n"
			      "Please be patient.\r\n");
		still_running = true;
	     }
	     cprintf("Memory limit = %10ld\r\n", max_mem);
	     cprintf("Allocated    = %10ld\r\n", current_mem);
	     cprintf("In use       = %10ld\r\n", inuse);
	     gotoxy(wherex(),wherey()-3);
	}
    }
#endif
#ifdef Mac
    ShowWait(max_mem,current_mem,inuse);
#endif

    GC_DEBUG("bytes	 reclaimed = %ld   in use = %ld\n", reclaimed, inuse);
    GC_DEBUG("blocks reclaimed = %ld   in use = %ld\n", unmarked, marked);
    GC_DEBUG("overhead %0.0f%%\n", 100*(got-wanted)/(float)wanted,0);

    if (gc_debug) {
	IPtr     p;
	long    count;
	double  sum;

	mcfflush(stdout);
	sum = count = 0;

	for (p = Free_List; p != Nil; p = Next_Free(p)) {
	    count++;
	    sum += Size(p);
	}

	mcprintf("Free Block Count = %ld Ave = %.2f \n",
	       count, sum / (count == 0 ? 1 : count)
	    );
	mcfflush(stdout);
    }
    if (biggest < needed || reclaimed < BYTES) {
	if (needed > BYTES) {
	    RT_ERROR("Internal object too large.");
	}
	if (!Add_Segment()) {
	    mcprintf("Data memory limit of %ld bytes exceeded\n", max_mem,0);
	    mcprintf("or real memory exceeded\n",0,0);
	    mcprintf("See manual under !memory to increase limit\n",0,0);
	    RT_ERROR("Allocated data memory exhausted");
	}
    }


#ifdef Signals
    Init_Signals();
#endif
}




/*
 * (n): Return integer with value n
 */
IPtr     New_Integer(n)
    WORKING n;
{
    IPtr     new;

    if (-BASE < n && n < BASE) {
	new = New_Block(Integer, 0);
	Int_Value(new) = n;
    } else {
	new = New_Big(n);
    }
    return new;
}


/*
 * (n): Return Boolean with value n
 */
IPtr     New_Boolean(n)
    Bool    n;
{
    return (n ? ITRUE : IFALSE);
}


/*
 * (n): Return File with value f
 */
IPtr     New_File(f, c)
    C_File  f;
    char    c;
{
    IPtr     new = New_Block(File, 0);

    File_Value(new) = f;
    File_Mode(new) = c;
    return new;
}

/*
 * (n): Return Real with value n
 */
IPtr     New_Real(n)
    C_Real  n;
{
    IPtr     new = New_Block(Real, 0);

    Real_Value(new) = n;
    return new;
}



IPtr RatNorm(num,den) IPtr num,den; {
	ENTER;

	Reg(ans);
	Reg(a); Reg(b); Reg(q); Reg(r);
	R(a) = num;
	R(b) = den;

	while( Length(R(b))!=0 ) {
	    mpUDivMod(R(a), R(b), &R(q), &R(r));
	    R(a) = R(b);
	    R(b) = R(r);
	    mpNorm(R(b));
	}

	R(ans) = New_Block(Rational, 0);
	mpDivMod(num, R(a), &Num(R(ans)), &R(r));

	mpDivMod(den, R(a), &Den(R(ans)), &R(r));

	if( Kind(Den(R(ans)))==Integer && Int_Value(Den(R(ans)))==1 ){
	    R(ans) = Num(R(ans));
	} else {
	    Num(R(ans)) = mpForce(Num(R(ans)));
	    Den(R(ans)) = mpForce(Den(R(ans)));
	    if( Sign(Den(R(ans)))==-1 ) {
	        Sign(Den(R(ans))) = 1;
	        Sign(Num(R(ans))) = -Sign(Num(R(ans)));
	    }
	}


	LEAVE R(ans);
    }


/*
 * (n,d): Return Rational n/d
 */
IPtr     New_Rational(n,d)
    IPtr n,d;
{
    ENTER;
    Reg(D);
    Reg(N);
    Reg(ans);

    R(N) = mpForce(n);
    R(D) = mpForce(d);

    R(ans) = RatNorm(R(N),R(D));
    LEAVE R(ans);
}


/*
 * (p): Return String with value string pointed to by p
 */
IPtr     New_String(p)
    C_Str   p;
{
    IPtr     new = New_Block(String, strlen(p));

    strcpy(Str_Value(new), p);
    return new;
}




/* (x,i): Return tuple with values in x, origin i */
IPtr     New_Tuple(x,i)
  IPtr     x;
  Idx i;
{
    ENTER;
    Reg     (ans);

    R(ans) = New_Block(Tuple, 0);
    Tuple_Origin(R(ans)) = i;
    switch (Kind(x)) {
      case List:
	Tuple_Value(R(ans)) = mcList_Tree(x);
	Tuple_Value(R(ans)) = mcTrim(Tuple_Value(R(ans)));
	break;

      default:
	Tuple_Value(R(ans)) = mcTrim(x);
    }
    LEAVE   R(ans);
}




/* (x): Return set with values in x */
IPtr     New_Set(x)
    IPtr     x;
{
    ENTER;
    Reg     (ans);
    Reg     (temp);

    switch (Kind(x)) {
      case List:{
	    R(temp) = NilHT;
	    for (; x != Nil; x = Next(x)) {
		mcNSHWith(Info(x), &R(temp));
	    }
	    R(ans) = New_Block(Set, 0);
	    if (R(temp) == OM)
		R(ans) = OM;
	    else
		Set_Value(R(ans)) = R(temp);
	} break;

      case HTree:{
	    R(ans) = New_Block(Set, 0);
	    Set_Value(R(ans)) = x;
	} break;


      default:			       /* singleton */
	R(ans) = New_Block(Set, 0);
	Set_Value(R(ans)) = x;
    }

    LEAVE   R(ans);
}


/* (left, right): tree */
IPtr     New_Tree(left, right)
    IPtr     left,
            right;
{
    ENTER;
    Reg     (ans);
    int     hl = mcHgt(left);
    int     hr = mcHgt(right);
    int     delta = hl - hr;


    assert(left != NilT);
    assert(right != NilT);
    assert(-max_delta <= delta && delta <= max_delta);

    R(ans) = New_Block(Tree, 0);
    Left(R(ans)) = left;
    Right(R(ans)) = right;
    Len(R(ans)) = mcTLen(left) + mcTLen(right);
    Hgt(R(ans)) = 1 + (hl > hr ? hl : hr);

    LEAVE   R(ans);
}

/* (HTree) assume subtrees balanced.  sets necessary fields */
void    HTFix(T)
    IPtr     T;
{
    IPtr     left = HLeft(T);
    IPtr     right = HRight(T);
    int     hl = mcHHgt(left);
    int     hr = mcHHgt(right);
    int     delta = hl - hr;

    assert(left != NilHT);
    assert(right != NilHT);
    assert(-max_delta <= delta && delta <= max_delta);

    HLen(T) = mcHTLen(left) + mcHTLen(right);
    HMax(T) = mcHMax(right);
    HHgt(T) = 1 + (hl > hr ? hl : hr);
}


/* (left, right): tree */
IPtr     New_HTree(left, right)
    IPtr     left,
            right;
{
    ENTER;
    Reg     (ans);

    R(ans) = New_Block(HTree, 0);
    HLeft(R(ans)) = left;
    HRight(R(ans)) = right;
    HTFix(R(ans));
    LEAVE   R(ans);
}
