#include	"common.h"
#include    "version.h"
#include	<ctype.h>
/* $Header: interp.c,v 3.2 90/04/25 16:22:52 gary Locked $ */
#include	"instr.h"
#include	"emitter.h"
#include	"manager.h"
#include	"ast.h"
#include	"op.h"
#include	"micro.h"
#include	"parser.h"
#include	"scanner.h"

#ifndef TURBOC
   /* TC has these in stdlib.h */
extern double atof(STR);
#endif

jmp_buf err_ret;


/* Global access to command line */
static int gargc;
static char **gargv;
static FILE *stdin1;
static Bool need_prompt = true;
       char *first_arg;


/* static forward declarations */
static void    Execute(NONE);
static int     Finput(NONE);
static void	ReadExecute(STR X BOOL X C_FILE X INT ADDR);

#define open_source(s) FOpen(s, "r")

/* Open the next source file on the command line.
 * Switch to stdin once sources are complete.
 */
FILE   *next_file()
{
    FILE   *f;

    f = FNULL;
    /* Try to open next file */
    while (f == FNULL && gargc > 0) {
	f = open_source(gargv[0]);
	if (f == FNULL) {
	    mcprintf("Cannot open '%s'\n", gargv[0]);
	}
	loading = true;
	silent = true;
	gargc--;
	gargv++;
    }

    /* If no more files, go to stdin (stored in stdin1) */
    if (f == FNULL && gargc == 0) {
	f = stdin1;
	loading = false;
	silent = false;
	gargc--;
    }
    return f;
}

static int leap = -1;   /* -1 ==> stop on each statement */
			/* >=0 ==> stop only when nested at level or less */
int call_level  =  0;


/* Real main program.  Invoked by shells (isetl.c or macmain.c)
 */
void    _main(argc, argv)
    int     argc;
    char  **argv;
{
    char    rev_version[9];	       /* extracted from rev_info	 */
    char    rev_date[9];	       /* also from rev_info		 */
    char    rev_time[9];	       /* also from rev_info		 */
    VOLATILE                        /* protect from longjmp */
      IPtr     Base_Env;		       /* Initial Environment */
    Bool    d_flag = false;
#ifdef TORTURE
    Bool    t_flag = false;
#endif

    first_arg = argv[0];

    yyin = stdin;

    gargc = argc - 1;
    gargv = argv + 1;
    if (gargc < 0)
	gargc = 0;
    while (gargc > 0 && gargv[0][0] == '-') {
	switch (gargv[0][1]) {
	  case 's':
	    silent_running = true;
	    break;
	  case 'd':
	    d_flag = true;
	    break;
#ifdef Screen
#ifdef TURBOC
	  case 'v':  {
	    extern int directvideo;
	    directvideo = 0;
	   } break;
#endif
#endif
#ifdef TORTURE
	  case 't': {
	      t_flag = true;
	  } break;
#endif
	  default:
	    mcprintf("Unknown flag '%s'\n", gargv[0]);
	    do_exit(GOOD_EXIT);
	}
	gargv++;
	gargc--;
    }

    direct_input = d_flag;
    if( !direct_input ) {

#ifdef Screen
	prompt_chars = ">?";
#ifdef TURBOC
	Einit(10000);
#endif
#else
#ifdef Raw
	start_raw();
#endif
	/* initialize the dispatch vectors */
	initialize(argv);
	ile();
#endif
    }


    if (!silent_running) {
	char   *form = "%*s %*s %s %s %s ";

	sscanf(index(rev_info, '$'), form,
	       rev_version, rev_date, rev_time);
    mcprintf("%s (%s)        (c) Gary Levin\n", VERSION, rev_date);
    mcprintf("Type `!credits' for more information.\n");
    mcprintf("Type `!quit' to exit.\n\n");

	if (!direct_input) {
#ifdef Screen
#ifdef TURBOC
    mcprintf("To send a region of code, TAG the first line with ^T,\n");
    mcprintf("    then type RETURN on last line.\n");
    mcprintf("Type ESC for menu.\n");
#endif
#ifdef Mac
    mcprintf("To send a region of code, select it and type RETURN.\n");
#endif
#else
 mcprintf("Interactive Line Editor active\n");
#endif
      }

    }
    Init_Operations();
    Init_Store();
    Init_Key();
    Init_Pd();
    Base_Env = Environment;
    EP_Ptr(PC) = Nil;

#ifdef TORTURE
    torture = t_flag;
#endif

    /* open .isetlrc for reading and establish locals for next_file */
    silent = true;
    loading = true;
    stdin1 = yyin;
    yyin = open_init("isetl");
    if (yyin == FNULL)
	yyin = next_file();


#ifdef Signals
    Init_Signals();
#endif

    while (true) {
	int     dummy;
        VOLATILE
        Bool    was_silent = silent_running;
#ifdef GRAPHICS
        Bool    was_Graphics = Graphics;
#endif

	if (setjmp(err_ret)) {	       /* Taken on error return */
	    IPtr    *p = Top;
#ifdef GRAPHICS
#ifdef TURBOC
            if( !direct_input && was_Graphics) {
                flip_mode();
            }
#endif
#endif

	    while (stack_dump && AStack != p) {
		if (Kind(*p) == Closure && Kind(Cl_SRC(*p)) == String) {
		    mcprintf("stack>>> %s\n", Str_Value(Cl_SRC(*p)));
		}
		p--;
	    }
            silent_running = was_silent;
	}
#ifdef TURBOC
    {  if( !direct_input && still_running ) {
	   unpopup();
	   still_running = false;
       }
    }
#endif

	mcMkEmpty();
	Environment = Base_Env;

	leap = -1;  /* start in crawl mode for debugging */
	reading = false;
	ReadExecute(">", false, FNULL, &dummy);
	if (R(Top) != Nil && R(Top)!=NonPrint) {
	    mcPrint(R(Top), stdout, true);
	    mcFlush(stdout);
	}
	mcPop();

	assert(mcIsEmpty());
	assert(Environment == Base_Env);
    }
}



/* Execute from PC until NoOp */
int interrupted;
static void Execute()
{
    call_level = 0;
    interrupted = 0;
    for (; EP_Ptr(PC) != Nil && !interrupted;) {
	C_Instr instr;

	if (Top >= &AStack[Max_Stack - Stack_Buffer]) {

	    /*
	     * Possible to overflow without detection if more than
	     * Stack_Buffer is pushed in one instruction. Not likely,
	     * but possible.
	     */
	    RT_ERROR("Stack Overflow");
	}
	switch (Kind(EP_Ptr(PC))) {
	  case SRC:
	    Last_Line = CPtr(EP_Ptr(PC));
	    GO_TO(CNext(EP_Ptr(PC)));
	    if (single_step && (leap == -1 || leap >= call_level) ) {
		int     command;

		Show_Line();
		mcputc('\n');
		if( leap != -1 ) leap = call_level;

		reset_line();
		do {
		    mcprintf("?>");

		    /* read command and dispose of rest of line */
		    while( isspace(command = catch()) && command !='\n' );
		    need_prompt = true;
		    reset_line();

		    if (command == 'f') {
			single_step = false;
			break;
		    } else if (command == 'l') {
			leap = call_level;
		    } else if (command == 'c') {
			leap = -1;
		    } else if (command == 'e') {
			int     dummy;

			single_step = false;

			reading = true;
			ReadExecute("!", false, FNULL, &dummy);
			reading = false;

			reset_line();
			if (R(Top) != Nil) {
			    mcPrint(R(Top), stdout, true);
			    mcFlush(stdout);
			}
			mcPop();
			single_step = true;
		    } else if (!isspace(command)) {
			mcprintf("'%c' not recognized\n", command);
		    }  else break;
		} while (true);
	    }
	    break;

	  case PUSH:
	    mcPush(CPtr(EP_Ptr(PC)));
	    if (trace_debug) {
		mcprintf("* Push Const = ");
		mcPrint(R(Top), stdout, true);
		mcFlush(stdout);
	    }
	    /* Fall through to go to */
	  case Join:
	    GO_TO(CNext(EP_Ptr(PC)));
	    break;
	  case Code:
	    al_Instr;
	    instr = *(C_Instr *) ((char *) CData(EP_Ptr(PC))
				  + EP_Offset(PC));
	    EP_Offset(PC) += Inst_Mult;
	    if (trace_debug) {
		mcprintf("* %s\n",
		       Inst_Map[(int) instr].Name);
	    }
	    Exec(instr);
	    break;
	  case Fork:
	    mcPLabel(CPtr(EP_Ptr(PC)));
	    GO_TO(CNext(EP_Ptr(PC)));
	    if (trace_debug) {
		mcprintf("* Push Label\n");
	    }
	    break;
	  default:
	    mcprintf("Bad code block Kind = %d(0x%x)\n",
		   Kind(EP_Ptr(PC)), Kind(EP_Ptr(PC)));
	    assert(0);
	}
    }
    if( interrupted ) {
    	on_int(1);
    }
}

/* (x,f, last) Reads object from f assigns it to x
 * if f is FNULL, uses current file (last is last char read from f)
 * returns last char read
 */
int    mcRead(x, f, last)
    IPtr     x;
    C_File  f;
    int     last;
{
    reading = true;
    ReadExecute("?", true, f, &last);
    reading = false;

    mcAssign(x, R(Top));
    mcPop();
    return last;
}


/* mcReadF(x,format,f) Reads object according to format from f
 * assigns it to x
 * if f is FNULL, uses current file
 */
static int Finput()
{
    int     c;

    while ((c = immediate_char()) == EOF) {
	if (yywrap())
	    return EOF;
    }
    return c;
}



/* (x,format,f, last) Reads object from f assigns it to x,
 * using format to control appearance.
 * if f is FNULL, uses current file (last is last char read from f)
 * returns last char read
 */
int     mcReadF(x, format, f, last)
    IPtr     x,
            format;
    C_File  f;
    int     last;
{
    ENTER;
    Reg     (val);
    int     max_col = 1000;	       /* maximum width of field read */
    Bool    exact = false;	       /* must read max_col characters.
				        * if not exact, strings that
				        * appear as numbers are converted
				        * to numbers. */
    char    temp[200];
    int     c,
            i;
    Bool    was_silent = silent_running;

    silent_running = true;	       /* don't echo input */

    reading = true;
    if (f != FNULL)
	fpush(f, last);

    switch (Kind(format)) {
      case Undefined:
	break;
      case Integer:
	max_col = Int_Value(format);
	if (max_col < 0) {
	    max_col = -max_col;
	    exact = true;
	    if (max_col > 200) {
                silent_running = was_silent;
		RT_ERROR("Exact format too big in readf");
	    }
	}
	break;
      default:
        silent_running = was_silent;
	RT_ERROR("Bad format in readf");
    }

    i = 0;
    if (!exact) {
	while ((c = Finput()) != EOF && isspace(c)) {
	    /* Keep reading */
	    Clear_Char();
	}
	while (c != EOF && i < 200 && max_col > 0 && !isspace(c)) {
	    temp[i] = c;
	    i++;
	    max_col--;
	    Clear_Char();
	    c = Finput();
	}
    } else {
	while (max_col > 0 && (c = Finput()) != EOF) {
	    temp[i] = c;
	    i++;
	    max_col--;
	    Clear_Char();
	}
    }

    temp[i] = EOS;
    geof = (i == 0);

    if (i == 200) {
        silent_running = was_silent;
	RT_ERROR("input item too long");
    }
    if (exact) {
	R(val) = New_String(temp);
    } else {
	int     token;
	int     sign = 0;

	i = 0;
	switch (temp[i]) {
	  case '0':
	  case '1':
	  case '2':
	  case '3':
	  case '4':
	  case '5':
	  case '6':
	  case '7':
	  case '8':
	  case '9':
	    token = T_Integer;
	    break;
	  case '+':
	    token = T_Integer;
	    sign = 1;
	    i++;
	    break;
	  case '-':
	    token = T_Integer;
	    sign = -1;
	    i++;
	    break;
	  default:
	    token = T_String;
	}
	while (token == T_Integer && isdigit(temp[i])) {
	    i++;
	}
	if (token == T_Integer && temp[i] == '.') {
	    token = T_Real;
	    i++;
	}
	if (token == T_Real) {
	    while (isdigit(temp[i])) {
		i++;
	    }
	    switch (temp[i]) {
	      case 'e':
	      case 'E':
	      case 'f':
	      case 'F':
		i++;
		if (temp[i] == '+' || temp[i] == '-') {
		    i++;
		}
		while (isdigit(temp[i])) {
		    i++;
		}
		if (!isdigit(temp[i - 1]))
		    token = T_String;
	    }
	}
	if (temp[i] != EOS)
	    token = T_String;
	i = 0;
	switch (token) {
	  case T_Integer:
	    if (sign != 0)
		i = 1;
	    R(val) = mpStrToInt(&(temp[i]));
	    if (sign < 0) {
		if (Kind(R(val)) == Integer) {
		    R(val) = New_Integer((WORKING)-Int_Value(R(val)));
		} else {
		    Sign(R(val)) = -1;
		}
	    } break;
	  case T_Real:
	    R(val) = New_Real(atof(temp));
	    break;
	  case T_String:
	    R(val) = New_String(temp);
	    break;
	  default:
	    assert(0);
	}
    }
    if (f != FNULL)
	last = fpop();

    reading = false;


    mcAssign(x, R(val));
    silent_running = was_silent;
    LEAVE   last;
}


/* ReadExecute(prompt, expr_only, f, *last)
 * Read statement/expression from file f
 * returns value of expr as top of stack (nil if not expr)
 * if f is FNULL, uses current file (*last is last char read from f)
 * update *last
 */
static void ReadExecute(new_prompt, expr_only, f, last)
    char   *new_prompt;
    Bool    expr_only;
    C_File  f;
    int    *last;
{
    AST_Ptr T;
    Exec_Point old_PC;
    Bool    is_expr = false;

    AST_Reset();
    Last_Line = Nil;

    mcPush(EP_Ptr(PC));
    EP_Ptr(old_PC) = EP_Ptr(PC);
    EP_Offset(old_PC) = EP_Offset(PC);

    if (f != FNULL)
	fpush(f, *last);
    inside = false;		       /* top level prompt flag */
    last_prompt = new_prompt;
    if (need_prompt)
	prompt(new_prompt);
    need_prompt = false;
    Clear();
    yyparse();
    Source_Flush();
    T = parsed;
    mcfflush(stdout);

    if (f != FNULL)
	*last = fpop();

    Start_Code();
    if (!expr_only && ast_kind(T) == T_Prog) {
	w_Prog(T);

    } else {
	IPtr     RT = T;

	while (ast_kind(RT) == T_Source) {
	    RT = ast_2(RT);
	}
	if (ast_kind(RT) == CALL && ast_kind(ast_1(RT)) != T_If) {
	    w_Expr(ast_1(RT));
	    is_expr = true;
	} else if (expr_only) {
	    RT_ERROR("Input must be an expression");
	} else {
	    w_Stmt(T, false);
	}
    }

    Stop_Code();


    Live_AST = Nil;

    GO_TO(New_Code);
    Execute();

    /* restore old state (can pop ptr to old_PC) */
    if (is_expr)
	opSwap();
    EP_Ptr(PC) = EP_Ptr(old_PC);
    EP_Offset(PC) = EP_Offset(old_PC);
    mcPop();

    if (mcIsEmpty()) {
	mcPush(Nil);
    }
}



/* Try to open .STRrc and STR.ini in current and `HOME' directories */
FILE   *open_init(base)
    char   *base;
{
    char    path[100];
    FILE   *result;
    char   *trim_at;

    strcpy(path, ".");
    strcat(path, base);
    strcat(path, "rc");
    if ((result = open_source(path)) != NULL)
	return result;

    strcpy(path, base);
    strcat(path, ".ini");
    if ((result = open_source(path)) != NULL)
	return result;

#ifdef Mac
    strcpy(path, ".");
    strcat(path, base);
    strcat(path, "rc");
    if ((result = fopen(path,"r")) != NULL)
	return result;

    strcpy(path, base);
    strcat(path, ".ini");
    if ((result = fopen(path,"r")) != NULL)
	return result;

#endif

#ifdef VMS
    strcpy(path, base);
    strcat(path, "ini:");
    if ((result = open_source(path)) != NULL)
	return result;

#endif

#ifdef Get_Env
#ifndef VMS
    strcpy(path, getenv("HOME"));
    strcat(path, "/.");
    strcat(path, base);
    strcat(path, "rc");
    if ((result = open_source(path)) != NULL)
	return result;
#endif

    strcpy(path, getenv("HOME"));
#ifndef VMS
    strcat(path, "/");
#endif
    strcat(path, base);
    strcat(path, ".ini");
    if ((result = open_source(path)) != NULL)
	return result;
#endif

#ifdef Exec_Path
    strcpy(path,first_arg);
    /* trim trailing name */
	trim_at = strrchr(path, '/');
	if (trim_at == 0) { trim_at = strrchr(path, '\\'); }
	if (trim_at != 0) { *trim_at = 0; }
    strcat(path, "/.");
    strcat(path, base);
    strcat(path, "rc");

    if ((result = open_source(path)) != NULL)
	return result;

    strcpy(path, first_arg);
    /* trim trailing name */
	trim_at = strrchr(path, '/');
	if (trim_at == 0) { trim_at = strrchr(path, '\\'); }
	if (trim_at != 0) { *trim_at = 0; }
    strcat(path, "/");
    strcat(path, base);
    strcat(path, ".ini");

    if ((result = open_source(path)) != NULL)
	return result;
#endif
    return NULL;
}


void no_prompt(b) Bool b; { silent = b; }
