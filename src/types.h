/* $Header: types.h,v 3.2 90/04/25 16:25:18 gary Locked $ */

#define AST_Ptr IPtr
#define Max_Line 200

/*
 * Definition of type tags and useful manipulations of same. Also contains
 * the definition of how these are represented in memory.
 *
 * When adding a new type, you must update mcPrint, mcEq, New_Block,
 * Mark_All.
 */

typedef char Type_Tags;

/* enum Type_Tags { */
#define		Atom		1
#define		Boolean		2
#define		Integer		3
#define		Bignum		4      /* Multiple precision */
#define		Real		5
#define         Rational        6
#define		Set		7
#define		String		8
#define		Tuple		9
#define		Undefined	10

#define		Code		11     /* All types for Code_Block */
#define		Join		12
#define		Fork		13
#define		PUSH		14     /* Push a constant */
#define         SRC             15

#define		Closure		16     /* <code,env,override> */
#define		Assoc		17     /* <var index, value> pair */

#define		Predef		18

#define		Label		19     /* IPtr to a Join */
#define		Iter		20     /* Holds refs to iter var */
                                       /* and remaining values */
#define		Ref		21     /* IPtr to Assoc for a var */
#define		Sel		22     /* Selector */
#define		Free		23     /* Unused Block */
#define		List		24     /* Linked list node */
#define		Tree		25     /* Tree node */
#define		HTree		26     /* Hash Tree node */
#define		AST		27
#define		File		28
#define         SArray          29
#define         TArray          30


/* }; */

typedef char Sel_Types;

/* enum Sel_Types { */
#define		Sel_SMap	1
#define		Sel_MMap	2
#define		Sel_Slice	3
/* }; */

typedef char Iter_Types;

/* enum Iter_Types { */
#define		I_In		1
#define		I_SMap		2
#define		I_MMap		3
#define		I_Cnt		4

#define		In_Str		5
#define		In_Set		6
#define		In_Tup		7

#define		F_Str		8
#define		F_Set		9
#define		F_Tup		10

#define		M_Str		11
#define		M_Set		12
#define		M_Tup		13
/* }; */

/* converts a pair of Type_Tags into an integer */
#define Pair(x,y)	( (x << 5) + y )





/* Definitions of blocks that represent ISETL objects */

typedef struct Block *IPtr;
typedef IPtr C_Label;

struct exec_pt {
    IPtr     ep_ptr;
    int     ep_offset;
};

typedef struct exec_pt Exec_Point;     /* Execution Point */

#define BASE_BITS	8
#define BASE		(1<<BASE_BITS)
typedef int DIGIT;
typedef long int WORKING;

struct Array_Block {
    int len;
    IPtr data[1];
};
struct Atom_Block {
    short   atom_value;
};
struct Bool_Block {
    short   boolean_value;
};
struct Int_Block {
    C_Int   integer_value;
};
struct File_Block {
    C_File  file_value;
    Bool    file_eof;
    char    file_mode,
            file_last;
};
struct Real_Block {
    C_Real  real_value;
};
struct Rational_Block {
    IPtr    numV, denV;
};
struct Undef_Block {
    char    OM_value;
};
struct BN_Block {
    int     sign,
            length;
    DIGIT   digits[1];
};



#define Id_Watched(p)		((p)->Variant.clVariant.id_watched)
/* Id_Watched refers to the ID that held this value,   */
/* if that ID is watched                             */
/* Needed for tuple,func,set(map),string,predef        */

/* Function like types */
struct Set_Block {
    C_Ref   id_watched;
    IPtr     set_value;
    int     set_hash;
};
struct Str_Block {
    C_Ref   id_watched;
    char    string_value[1];
};
struct Tuple_Block {
    C_Ref   id_watched;
    Idx	    tuple_origin;
    IPtr     tuple_value;
    int     tuple_hash;
};
struct Close_Block {
    C_Ref   id_watched;
    IPtr     cl_code;
    IPtr     cl_env;
    IPtr     cl_override;
/* use internal set rep */
    IPtr     cl_src;
    int	     cl_cnt;
};
struct Pd_Block {
    C_Ref   id_watched;
    C_Pd    (pd_code);
    short   arg_count;
    char   *pd_name;
};



#define Scale_Up(x)	((WORKING)(x) << BASE_BITS)
#define Low(x)		((x)&(BASE-1))

#ifdef LShift
#	define High(x)	(x<0 ? ~((~(x))>>BASE_BITS) : (x)>>BASE_BITS)
#else
#	define High(x)	((x)>>BASE_BITS)
#endif

/* Make sure that Low and High give correct results for x<0 */
/* In particular, 0<=Low<BASE, High*BASE+Low == x */



/*
 * The following definitions define the four variants of Code_Blocks.
 */
struct J_Block {
    IPtr     cnext;
};
struct C_Block {
    IPtr     cnext;
    union {
	char    cdata[Max_Code
	        /* This provides space for instr+data */ + 2 * Gen_Mult];
	 /* Forcing general alignment */ C_Real CB1;
	C_Int   CB2;
	IPtr     CB3;
    }       CAlign;
};

/* the union in C_Block is to force general alignment */
struct F_Block {
    IPtr     cnext;
    IPtr     Fcdata;
};

/* Use F_Block for P_Block */


struct Assoc_Block {
    C_Ref   var_index;
    IPtr     var_value;
};
struct Lab_Block {
    C_Label lab_value;
};
struct Iter_Block {
    IPtr     i_LHS;
    IPtr     i_Values;
    IPtr     i_Rest;
    C_Int   i_cnt;
    Iter_Types i_kind;
};
struct List_Block {
    IPtr     info;
    IPtr     next;
};
struct Tree_Block {
    IPtr     tleft;
    IPtr     tright;
    Idx     len;
    short   hgt;
};
struct HTree_Block {
    IPtr     htleft;
    IPtr     htright;
    int     hmax;
    Idx     hlen;
    short   hhgt;
};
struct AST_Block {
    AST_Ptr AST_More;
    AST_Ptr AST_1,
            AST_2;
    int     AST_Kind;
};
struct Free_Block {
    IPtr     next_free;
};
struct Ref_Block {
    IPtr     var_assoc;
    IPtr     sel_list;
};
struct Sel_Block {
    Sel_Types sel_is;
    IPtr     at_index,
            to_index;
    IPtr     sel_next;
};

/*
 * Sel_List is a Nil terminated list of Sel_Blocks linked through
 * Sel_Next. If Sel_Is = Sel_SMap or Sel_MMap, only At_Index is defined;
 * o.w. selector is a slice and both At_Index and To_Index are defined.
 */

#define Array_Len(p)            ((p)->Variant.ARVariant.len)
#define Array_Data(p)           ((p)->Variant.ARVariant.data)
#define Atom_Value(p)		((p)->Variant.aVariant.atom_value)
#define Bool_Value(p)		((p)->Variant.bVariant.boolean_value)
#define Int_Value(p)		((p)->Variant.iVariant.integer_value)
#define File_Value(p)		((p)->Variant.fileVariant.file_value)
#define File_Eof(p)		((p)->Variant.fileVariant.file_eof)
#define File_Mode(p)		((p)->Variant.fileVariant.file_mode)
#define File_Last(p)		((p)->Variant.fileVariant.file_last)
#define Real_Value(p)		((p)->Variant.rVariant.real_value)
#define Num(p)			((p)->Variant.ratVariant.numV)
#define Den(p)			((p)->Variant.ratVariant.denV)
#define Set_Value(p)		((p)->Variant.sVariant.set_value)
#define Set_Hash(p)		((p)->Variant.sVariant.set_hash)
#define Str_Value(p)		((p)->Variant.strVariant.string_value)
#define Tuple_Origin(p)		((p)->Variant.tVariant.tuple_origin)
#define Tuple_Value(p)		((p)->Variant.tVariant.tuple_value)
#define Tuple_Hash(p)		((p)->Variant.tVariant.tuple_hash)

#define Digits(p)		((p)->Variant.bnVariant.digits)
#define Length(p)		((p)->Variant.bnVariant.length)
#define Sign(p)			((p)->Variant.bnVariant.sign)

#define CNext(p)		((p)->Variant.cVariant.cnext)
#define CData(p)		((p)->Variant.cVariant.CAlign.cdata)
#define CPtr(p)			((p)->Variant.fbVariant.Fcdata)

#define Cl_Code(p)		((p)->Variant.clVariant.cl_code)
#define Cl_Env(p)		((p)->Variant.clVariant.cl_env)
#define Cl_SRC(p)		((p)->Variant.clVariant.cl_src)
#define Cl_Override(p)		((p)->Variant.clVariant.cl_override)
#define Cl_Cnt(p)		((p)->Variant.clVariant.cl_cnt)

#define Var_Index(p)		((p)->Variant.asVariant.var_index)
#define Var_Value(p)		((p)->Variant.asVariant.var_value)
#define Var_Assoc(p)		((p)->Variant.Ref_Variant.var_assoc)

#define Pd_Code(p)		((p)->Variant.pdVariant.pd_code)
#define Pd_argc(p)		((p)->Variant.pdVariant.arg_count)
#define Pd_name(p)		((p)->Variant.pdVariant.pd_name)

#define Label_Value(p)		((p)->Variant.Lab_Variant.lab_value)

#define Iter_LHS(p)		((p)->Variant.Iter_Variant.i_LHS)
#define Iter_Source(p)		((p)->Variant.Iter_Variant.i_Values)
#define Iter_Rest(p)		((p)->Variant.Iter_Variant.i_Rest)
#define Iter_Cnt(p)		((p)->Variant.Iter_Variant.i_cnt)
#define Iter_Kind(p)		((p)->Variant.Iter_Variant.i_kind)

#define Info(p)			((p)->Variant.List_Variant.info)
#define Next(p)			((p)->Variant.List_Variant.next)

#define Len(p)			((p)->Variant.Tree_Variant.len)
#define Hgt(p)			((p)->Variant.Tree_Variant.hgt)
#define Left(p)			((p)->Variant.Tree_Variant.tleft)
#define Right(p)		((p)->Variant.Tree_Variant.tright)

#define HLen(p)			((p)->Variant.HTree_Variant.hlen)
#define HMax(p)			((p)->Variant.HTree_Variant.hmax)
#define HHgt(p)			((p)->Variant.HTree_Variant.hhgt)
#define HLeft(p)		((p)->Variant.HTree_Variant.htleft)
#define HRight(p)		((p)->Variant.HTree_Variant.htright)

#define ast_1(p)		((p)->Variant.AST_Variant.AST_1)
#define ast_2(p)		((p)->Variant.AST_Variant.AST_2)
#define ast_kind(p)		((p)->Variant.AST_Variant.AST_Kind)
#define ast_more(p)		((p)->Variant.AST_Variant.AST_More)

#define Next_Free(p)		((p)->Variant.fVariant.next_free)
#define Kind(p)			((p)->header.kind)
#define Mark(p)			((p)->header.mark)
#define Size(p)			((p)->header.size)

#define Sel_List(p)		((p)->Variant.Ref_Variant.sel_list)
#define Sel_Is(p)		((p)->Variant.Sel_Variant.sel_is)
#define At_Index(p)		((p)->Variant.Sel_Variant.at_index)
#define To_Index(p)		((p)->Variant.Sel_Variant.to_index)
#define Sel_Next(p)		((p)->Variant.Sel_Variant.sel_next)

#define EP_Ptr(p)		((p).ep_ptr)
#define EP_Offset(p)		((p).ep_offset)


union variants {
    struct Array_Block ARVariant;
    struct Atom_Block aVariant;
    struct Bool_Block bVariant;
    struct Int_Block iVariant;
    struct BN_Block bnVariant;
    struct File_Block fileVariant;
    struct Real_Block rVariant;
    struct Rational_Block ratVariant;
    struct Set_Block sVariant;
    struct Str_Block strVariant;
    struct Tuple_Block tVariant;
    struct Undef_Block Undef_Variant;

    struct C_Block cVariant;
    struct J_Block jVariant;
    struct F_Block fbVariant;
    struct Close_Block clVariant;
    struct Assoc_Block asVariant;
    struct Pd_Block pdVariant;
    struct Lab_Block Lab_Variant;
    struct Iter_Block Iter_Variant;
    struct Ref_Block Ref_Variant;
    struct Sel_Block Sel_Variant;
    struct List_Block List_Variant;
    struct Tree_Block Tree_Variant;
    struct HTree_Block HTree_Variant;
    struct AST_Block AST_Variant;
    struct Free_Block fVariant;
};

struct Block_Head {
    Type_Tags kind;
    char    mark;
    short   size;
};

/*
 * Assumes that C_Real is the largest and most difficult to align single
 * data object
 */
struct Mini_Block {
    struct Block_Head mini_head;
    C_Real  mini_dummy;
};

/* allocates enough space for constants (see Globals.c) */
struct Const_Block {
    struct Block_Head const_head;
    union {
	struct Atom_Block CaVariant;
	struct Bool_Block CbVariant;
	struct Int_Block CiVariant;
	struct BN_Block CbnVariant;
	struct Real_Block CrVariant;
	struct Undef_Block CUndef_Variant;

	struct List_Block CList_Variant;
	struct Tree_Block CTree_Variant;
	struct HTree_Block CHTree_Variant;
	struct AST_Block CAST_Variant;
	struct Free_Block CfVariant;
    }       const_variants;
};

struct Block {
    struct Block_Head header;
    union variants Variant;
};
