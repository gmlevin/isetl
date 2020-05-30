#include "common.h"
#include "ast.h"
#include "instr.h"

/* $Header: globals.c,v 3.1 90/04/25 16:22:29 gary Locked $ */
/* Definitions of all global names */
char   *CNULL = 0;
FILE   *FNULL = 0;

#ifdef InitCast
#	define	PtrCast (IPtr)
#else
#	define	PtrCast
#endif

struct Const_Block
        NilT_Block,
        NilHT_Block,
        Nil_Block,
        OM_Block,
        NonPrint_Block,
        FALSE_Block,
        TRUE_Block,
        BigOne_Block,
        BigTwo_Block,
        BigTen_Block;

IPtr     NonPrint = PtrCast &NonPrint_Block;
IPtr     NilT = PtrCast & NilT_Block;
IPtr     NilHT = PtrCast & NilHT_Block;
IPtr     Nil = PtrCast & Nil_Block;
IPtr     OM = PtrCast & OM_Block;
IPtr     ITRUE = PtrCast & TRUE_Block;
IPtr     IFALSE = PtrCast & FALSE_Block;
IPtr     BigOne = PtrCast & BigOne_Block;
IPtr     BigTwo = PtrCast & BigTwo_Block;
IPtr     BigTen = PtrCast & BigTen_Block;


AST_Ptr Source_List;
AST_Ptr Func_List;
AST_Ptr Func_Tail;
int     func_level;

IPtr     Environment;
IPtr     AStack[Max_Stack];	       /* Don't use AStack[0] */
IPtr    *Top;			       /* *Top == Last IPtr pushed. Will
				        * try to leave Stack_Buffer as
				        * room for overflow */


AST_Ptr parsed;
C_Ref   Anons = 0;		       /* High water mark for anonymous
				        * vars --- Anons..-1 are in use in
				        * current Env */


IPtr     New_Code,
        Tail_Code;		       /* Code Queue */
Exec_Point PC;
struct Block Working_Code;

long    max_mem = 104000L;
long    current_mem = 0;
char   *last_prompt = ">";

Bool    gc_debug = false;
Bool    instruction_printing = false;
Bool    trace_debug = false;
Bool    loading = false;
Bool    silent = false;
Bool    inside = false;
Bool    silent_running = false;
Bool    direct_input = true;
Bool    echo_mode = false;
Bool    store_source = true;
Bool    stack_dump = true;
Bool    single_step = false;
Bool    setrandom   = true;
Bool    reading     = false;   /* true during Read and Readf */
Bool	still_running = false;
Bool    do_rational   = false;
IPtr     Last_Line;
