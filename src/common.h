#include "machine.h"
/* $Header: common.h,v 3.1 90/04/25 16:24:40 gary Locked $ */

#ifndef EOF
#	include <stdio.h>
#endif

#include <stdlib.h>
#include <setjmp.h>

#include <math.h>

#define Bool	int
#define true	1
#define false	0
#define C_Pd(x)		IPtr (*x)()


/* Handle BSD vs Sys5 naming convention */

#ifdef StringsH
#	include <strings.h>
#endif

#ifdef StringH
#	include <string.h>
#endif


#ifdef Assert
#	include <assert.h>

#ifdef TURBOC
void _Cdecl abort(void);

#endif

#else
#if defined(Mac)&&defined(Screen)
#	define assert(x)	\
		if(!(x)) {assert_exit(__FILE__, __LINE__);}else
#else
#	define assert(x)	\
		if(!(x)) {mcprintf("assert failed %s %s\n",__FILE__, __LINE__);}else
#endif
#endif


/* Define types of C objects for use in ISETL */
typedef int C_Int;
typedef long int Idx;
typedef FILE *C_File;

#define MathReal	double

#ifdef Double
typedef double C_Real;

#else
typedef float C_Real;

#endif

typedef char *C_Str;
typedef short C_Ref;
typedef char C_Instr;


/*
 * Data of type X must begin on addresses that are multiples of X_Mult
 */

#ifndef Inst_Mult
#define Inst_Mult	sizeof(C_Instr)
#endif

#ifndef Int_Mult
#define Int_Mult	sizeof(C_Int)
#endif

#ifndef Real_Mult
#define Real_Mult	sizeof(C_Real)
#endif

#ifndef Char_Mult
#define Char_Mult	sizeof(char)
#endif

#ifndef Lab_Mult
#define Lab_Mult	sizeof(C_Label)
#endif


#ifndef Ref_Mult
#define Ref_Mult	sizeof(C_Ref)
#endif


#ifdef Void
#	define	void	int
#endif

/* Some systems define constants ITRUE and IFALSE.  Needed as ISETL constants */

#ifdef ITRUE
#	undef	ITRUE
#	undef	IFALSE
#endif


#define Quote		'"'
#define Set_Open	'{'
#define Tuple_Open	'['
#define Set_Close	'}'
#define Tuple_Close	']'

#define Max_Stack	1000
#define Stack_Buffer	30
#define Max_Code	(250)


/* This assumes that C_Real is the largest of all atomic types */

#ifndef Gen_Mult
#   define Gen_Mult	Real_Mult
#endif

/* Definitions for Prototyping */

#ifdef Prototype
#define X	,
#define PTR	IPtr
#define ADDR	*
#define INT	int
#define IDX	Idx
#define C_INT	C_Int
#define PWORKING WORKING
#define PDIGIT	DIGIT
#define NONE	void
#define C_FILE	C_File

/* prototypes must be widened to match oldstyle definitions */
#define BOOL	int
#define CHAR    int
#define C_REF	int
#define C_INSTR int
#define TAGS	int
#define C_REAL	double


#define STR	C_Str
#define C_LABEL C_Label
#define AST_PTR AST_Ptr
#define MORE	...
#define MATHFUNC	MathReal(*f)(MathReal)
#define PTRFUNC		IPtr(*f)(MathReal)
#define MATHREAL	MathReal
#define ARRAY		[]


#else
#define X
#define PTR
#define ADDR
#define INT
#define IDX
#define C_INT
#define PWORKING
#define PDIGIT
#define NONE
#define BOOL
#define C_FILE
#define CHAR
#define STR
#define C_REAL
#define TAGS
#define C_REF
#define C_INSTR
#define C_LABEL
#define AST_PTR
#define MORE
#define MATHFUNC
#define MATHREAL
#define PTRFUNC
#define ARRAY
#endif


#include "types.h"
#include "debug.h"
#include "mp.h"

extern FILE *fopen();
#ifdef Mac
extern FILE *FOpen();
#else
#define FOpen(f,m) fopen(f,m)
#endif

extern void do_exit(INT);

extern FILE *FNULL;
extern char *CNULL;

#ifndef EOS
#	define	EOS	'\0'
#endif


#ifdef FileExt
#	define	MAX_FN	12
#endif

#ifndef FileExt
#	define	MAX_FN	20
#endif

extern jmp_buf err_ret;
extern void Print_Id(C_REF);
extern int mcSetMaxLine(INT);

extern IPtr NilT;
extern IPtr NilHT;
extern IPtr Nil;
extern IPtr OM;
extern IPtr NonPrint;
extern IPtr ITRUE;
extern IPtr IFALSE;
extern IPtr BigOne;
extern IPtr BigTwo;
extern IPtr BigTen;

extern IPtr Environment;
extern IPtr AStack[];
extern IPtr *Top;

extern AST_Ptr parsed;
extern C_Ref Anons;
extern IPtr New_Code,
        Tail_Code;		       /* Code Queue */
extern Exec_Point PC;
extern struct Block Working_Code;

extern long max_mem;
extern long current_mem;
extern char *last_prompt;
extern int r_prec;


extern Bool gc_debug;
extern Bool instruction_printing;
extern Bool trace_debug;
extern Bool loading;
extern Bool silent;
extern Bool inside;
extern Bool silent_running;
extern Bool direct_input;
extern Bool echo_mode;
extern Bool single_step;
extern Bool setrandom;
extern Bool reading;
extern Bool do_rational;
extern Bool last_nl;		       /* controls \n on output (edit.c) */
extern FILE *CONin;		       /* Console input file  */
extern FILE *record;                   /* Recording file */
extern char* first_arg;                /* argv[0] */
extern Bool geof;



extern int  call_level;

extern void Init_Signals(NONE);
extern void Init_Key(NONE);	       /* keywords.c */
extern void Init_Pd(NONE);	       /* predef.c   */
extern void initialize(STR ARRAY);     /* ile.c */
extern void ile(NONE);		       /* ile.c */

#ifdef GRAPHICS
extern Bool Graphics;
extern void pdXGraphics(BOOL);
#endif

extern void Hold_Signals(NONE);
extern int yyparse(NONE);
extern Bool Add_Segment(NONE);

extern void reset_line(NONE);
extern void no_prompt(BOOL);

#ifndef Screen
	extern int ile_getc(NONE);
#endif

extern void on_int(INT);
extern void start_raw(NONE);
extern void _main(INT X STR ADDR);

#ifdef TURBOC

#else
#ifdef Mac

#else
#ifdef Vax

#else

#endif
#endif
#endif

#ifdef Screen
#ifdef TURBOC
extern char io_temp[];
#  define  mcputc(c)			if(direct_input) putchar(c);\
					else Eputc(c)
#  define  mcfputc(c,f)			{if(f==stdout)mcputc(c);\
					 else         putc(c,f);}
#  define  mcprintf(...)		{sprintf(io_temp, __VA_ARGS__);\
                                         mcEputs();}
#  define  mcfprintf(f,...)	        {sprintf(io_temp, __VA_ARGS__);	\
                                         mcEfputs(f);}
#  define  mcfflush(f) (f == stdout && !direct_input ? Eflush() : (void)fflush(f))
extern void mcEputs(NONE), mcEfputs(C_FILE), Eputc(CHAR);
#endif
#ifdef Mac
extern char io_temp[];
#  define  mcputc(c)			Eputc(c)
#  define  mcfputc(c,f)			{if(f==stdout)mcputc(c);\
					 else         putc(c,f);}
#  define  mcprintf(...)		{sprintf(io_temp, __VA_ARGS__);\
                                         mcEputs();}
#  define  mcfprintf(f,...)	{sprintf(io_temp, __VA_ARGS__);\
                                         mcEfputs(f);}
#  define  mcfflush(f) (f == stdout ? Eflush() : fflush(f))
extern void mcEputs(NONE), mcEfputs(C_FILE), Eputc(CHAR);
#endif
#else
#  define  mcputc(c)			putchar(c)
#  define  mcfputc(c,f)			putc(c,f)
#  define  mcprintf(...)		printf(__VA_ARGS__)
#  define  mcfprintf(f,...)	        fprintf(f, __VA_ARGS__)
#  define  mcfflush(f) fflush(f)
#endif

#ifdef Index
#  define index strchr
#endif
#ifndef TURBOC
char * index(STR X CHAR);
#endif

#ifdef Screen
#   ifdef TURBOC
#   include "editor.h"
#   endif
#   ifdef Mac
      extern char * prompt_chars;
#	endif
#endif
	

Bool is_num(PTR);
Bool is_big(PTR);
Bool is_rat(PTR);
