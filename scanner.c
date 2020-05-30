
#
/* $Header: scanner.c,v 3.2 90/04/25 16:24:18 gary Locked $ */
#   include	"parser.h"
#   include	"common.h"
#   include     "scanner.h"
#   include     "symbol.h"
#   include	"manager.h"
#   include	<ctype.h>
#define		MAX_LINE	100
#define		RETURN		inside = 1; return
#define         B_RETURN(x)     yylval = (AST_Ptr)(x); RETURN x
#define		MAX_INCLUDE	10
int     line_num = 1;

/* forward static declarations */
void alias(STR X STR);
char *in_alias(STR);

static int scan_error;
static int top = 0;
static FILE *stack[MAX_INCLUDE];
static int ln_stack[MAX_INCLUDE];
static int bk_stack[MAX_INCLUDE];
static char fn_stack[MAX_INCLUDE][MAX_FN] = {""};
FILE   *yyin;

int     Bang(STR);
int     Key(STR);


#define MAX_INPUT 400
char    yytext[MAX_INPUT];

int     backup[10],
        saved = 0;
int     last_read = 0;

int     New_Char()
{
    if (saved != 0) {
	saved--;
	last_read = backup[saved];
    } else
	last_read = catch();
    return last_read;
}

int     Refresh_Char()
{
    if (last_read == 0)
	New_Char();
    return last_read;
}

void    Clear_Char()
{
    last_read = 0;
}

/* Assumes that only need backup when reading expressions */
/* Otherwise, one char (last_read) suffices, which can be saved */
/* in File_Last */
void    unget(INT);
void    unget(c)
    int     c;
{
    assert(saved < 10);
    backup[saved] = c;
    saved++;
}

char   *intext;
void    Save(CHAR);
static void Save(c)
    char    c;
{
    if (intext >= yytext + MAX_INPUT - 1) {
	SYNTAX("Token too long");
	scan_error = true;
	intext = yytext;
    } else {
	*intext = (c);
	intext++;
    }
}

int     yylex()
{

    for(;;) {
	scan_error = false;
	intext = yytext;
	Refresh_Char();
	switch (last_read) {
	  case EOF:{
		if (yywrap()) {	       /* simulate ``OM;'' */
		    last_read = ';';
		    RETURN  T_Om;
		} else {
		    Clear_Char();
		}
	    } break;

	  case '!':{
		int     result;

		while (last_read != '\n' && last_read != EOF) {
		    Save(last_read);
		    New_Char();
		}
		Save(EOS);
		result = Bang(yytext);
		if (result != 0)
		    return result;
	    } break;

	  case '$':{
		while (last_read != '\n' && last_read != EOF) {
		    New_Char();
		}
	    } break;

	  case '\f':
	  case ' ':
	  case '\t':
	  case '\r':
	    Clear_Char();
	    break;

	  case '\n':{
		Clear_Char();
		line_num++;
		prompt(last_prompt);
	    } break;

	  case '=':{
		Clear_Char();
		B_RETURN(  T_Eq);
	    }
	  case '%':{
		Clear_Char();
		RETURN  T_Insert;
	    }
	  case '-':{
		New_Char();
		if (last_read == '>') {
		    Clear_Char();
		    RETURN  T_Arrow;
		} else {
		    B_RETURN(  T_Minus);
		}
	    }
	  case '~':{
		Clear_Char();
		RETURN  T_Missing;
	    }
	  case '+':{
		Clear_Char();
		B_RETURN(  T_Plus);
	    }

	  case '@':
	  case ',':
	  case ';':
	  case '#':
	  case '?':
	  case '(':
	  case ')':
	  case '[':
	  case ']':
	  case '{':
	  case '}':
	    {
		char    c = last_read;

		Clear_Char();
		RETURN  c;
	    }

	  case '|':{
		Clear_Char();
		RETURN  T_St;
	    }

	  case '*':{
		New_Char();
		if (last_read == '*') {
		    Clear_Char();
		    B_RETURN(  T_Exp);
		} else {
		    B_RETURN(  T_Times);
		}
	    }

	  case '.':{
		New_Char();
		if (last_read == '.') {
		    Clear_Char();
		    RETURN  T_Dotdot;
		} else if( isdigit(last_read) ) {
		    unget(last_read);
		    last_read = '.';
		    goto read_number;
		} else {
		    RETURN '.';
		}
	    }

	  case '/':{
		New_Char();
		if (last_read == '=') {
		    Clear_Char();
		    B_RETURN(  T_Ne);
		} else {
		    B_RETURN(  T_Slash);
		}
	    }

	  case ':':{
		New_Char();
		if (last_read == '=') {
		    Clear_Char();
		    RETURN  T_Gets;
		} else {
		    RETURN  T_St;
		}
	    }

	  case '<':{
		New_Char();
		if (last_read == '=') {
		    Clear_Char();
		    B_RETURN(  T_Le);
		} else {
		    B_RETURN(  T_Lt);
		}
	    }

	  case '>':{
		New_Char();
		if (last_read == '=') {
		    Clear_Char();
		    B_RETURN(  T_Ge);
		} else {
		    B_RETURN(  T_Gt);
		}
	    }



	  case '\'':
	  case '"':{		       /* yylval = string without quotes */
		char    open_quote = last_read;

		New_Char();
		yylval = (AST_Ptr)yytext;
		while (1) {
		    switch (last_read) {
		      case '\n':
			SYNTAX("Embedded newline in string");
			/* pretend that you read close quote */
		      case '"':
		      case '\'':
			if (last_read == '\n'
			    || last_read == open_quote) {
			    Save(EOS);
			    Clear_Char();
			    RETURN  T_String;
			} else {
			    Save(last_read);
			    break;
			}

		      case '\\':
			New_Char();
			switch (last_read) {
			  case 'n':
			    Save('\n');
			    break;
			  case 't':
			    Save('\t');
			    break;
			  case 'q':
			    Save('"');
			    break;
			  case 'b':
			    Save('\b');
			    break;
			  case 'r':
			    Save('\r');
			    break;
			  case 'f':
			    Save('\f');
			    break;
			  default:{
				if ('0' <= last_read
				    && '7' >= last_read) {
				    int     val,
				            i;

				    i = 0;
				    val = 0;
				    while (i < 3 &&
					   '0' <= last_read
					   && '7' >= last_read) {
					val = val * 8 + last_read - '0';
					i++;
					New_Char();
				    }
				    if (val < 1 || val > 255) {
					SYNTAX("bad octal char");
				    } else {
					Save(val);
				    }
				    continue;
				} else {
				    Save(last_read);
				}
			    } break;
			} break;

		      default:
			Save(last_read);
			break;
		    }
		    New_Char();
		}
	    }

	  default:
	  read_number:
	    if (isalpha(last_read)) {
		int     token;

		while (isalnum(last_read) || last_read == '_'
		       || last_read == '\'' || last_read == '^' ) {
		    Save(last_read);
		    New_Char();
		}
		Save(EOS);
		token = Key(yytext);
		if (token == 0) {
		    yylval = (AST_Ptr)yytext;
		    RETURN  T_Id;
		} else {
		    B_RETURN(  token);
		}
	    } else if (isdigit(last_read) || last_read=='.') {
		int     token = T_Integer;

		inside = true;
		yylval = (AST_Ptr)yytext;
		while (isdigit(last_read) || last_read == '\\') {
		    if (last_read == '\\') {
			New_Char();
			while (isspace(last_read)) {
			    if (last_read == '\n') {
				line_num++;
				prompt(last_prompt);
			    }
			    New_Char();
			}
		    } else {
			Save(last_read);
			New_Char();
		    }
		}
		/* check for '.' */
		if (last_read == '.') {
		    /* check for .. */
		    New_Char();
		    if (last_read == '.') {
			unget('.');
		    } else
			token = T_Real;
		}
		if (token == T_Real) {
		    Save('.');
		    while (isdigit(last_read)) {
			Save(last_read);
			New_Char();
		    }
		    switch (last_read) {
		      case 'e':
		      case 'E':
		      case 'f':
		      case 'F':
			Save('e');
			New_Char();
			if (last_read == '+' || last_read == '-') {
			    Save(last_read);
			    New_Char();
			}
			while (isdigit(last_read)) {
			    Save(last_read);
			    New_Char();
			}
			if (!isdigit(*(intext - 1))) {
			    SYNTAX("Badly formed float");
			    Save('0');
			}
		    }
		}
		Save(EOS);
		RETURN  token;
	    } else {
		mcprintf("char '%c(0x%02x)' skipped\n",
		       last_read, last_read);
		Save(last_read);

		New_Char();
	    }
	}
    }
}






/* This code (fpush and fpop) handle nestings due to "read" commands */
int pushes = 0;
Bool    geof;

void    fpush(f, c)
    C_File  f;
    int     c;
{
    stack[top] = yyin;
    ln_stack[top] = line_num;
    bk_stack[top] = last_read;
    last_read = c;
    line_num = 1;
    top++;
    if (top == MAX_INCLUDE) {
	FATAL("Includes too deeply nested");
    }
    fn_stack[top][MAX_FN - 1] = '\0';
    strncpy(fn_stack[top], "(Unknown)", MAX_FN - 1);

    yyin = f;
    pushes++;
}

int     fpop()
{
    int     old = last_read;

    --top;
    yyin = stack[top];
    line_num = ln_stack[top];
    last_read = bk_stack[top];
    saved = 0;

    if (pushes > 0)
	pushes--;
    return old;
}


/* Clear out all current inputs */
void    yyclear()
{
    while (pushes > 0)
	fpop();
    while (top > 0) {
	fclose(yyin);
        geof = false;
	fpop();
	if (echo_mode || (!silent && !silent_running && top == 0)) {
	    mcprintf("!include %s completed\n", fn_stack[top + 1],0);
	}
    }
    reset_line();
    inside = false;
    prompt(last_prompt);
}


/* 0 means keep reading, 1 means simulate reading ``OM;'' */
int     yywrap()
{
    if (pushes > 0) {
	geof = true;
	return 1;
    }
    if (!loading && top == 0) {
	mcputc('\n');
	do_exit(GOOD_EXIT);
    }
    if (loading && top == 0) {
	fclose(yyin);
	yyin = next_file();

	line_num = 1;
	if( !echo_mode ) prompt(last_prompt);
	return 0;
    } else {
	fclose(yyin);
	fpop();
	if (echo_mode || (!silent && !silent_running && top == 0)) {
	    mcprintf("!include %s completed\n", fn_stack[top + 1],0);
	}
	prompt(last_prompt);
	return 0;
    }
}

void    prompt(s)
    char   *s;
{
#ifdef ScreenX
#ifdef GRAPHICS
#ifdef TURBOC
    if( Graphics ) {
	RT_ERROR("Attempt to read at prompt in graphics mode.");
    }
#endif
#endif
#endif
    if (echo_mode || (!silent && !silent_running && top == 0)) {
#ifdef Screen
	if( !direct_input ) nl_at_end();
#endif
#ifdef Mac
    HideWait();
#endif

	if (inside) {
	    mcprintf("%s%s      ", s, s);
	} else {
	    mcprintf("%s       ", s,0);
	}
    }
    last_prompt = s;
    mcfflush(stdout);
}


/* Return pointer to first non-space (or EOS) at or after s. */
char   *skip_space(STR);
static char *skip_space(s)
    char   *s;
{
    while (*s && isspace(*s))
	s++;
    return s;
}

/* Return pointer to first space (or EOS) at or after s. */
char   *find_space(STR);
static char *find_space(s)
    char   *s;
{
    while (*s && !isspace(*s))
	s++;
    return s;
}

Bool    good_id(STR);
static Bool good_id(s)
    char   *s;
{
    if (!isalpha(*s))
	return false;
    while (*s != EOS) {
	if (!(*s == '_' || isalnum(*s)))
	    return false;
	s++;
    }
    return true;
}

/* Process ! commands.	Return 0 to ignore the command. */
FILE   *record = NULL;

void toggle(INT ADDR);

static char ON[] = "on\0";
static char OFF[] = "off\0";
    
static    char   *command,
           *com_end,
           *arg,
           *arg_end;

static FILE * pp_out = 0;


void toggle(var) Bool *var; {
    if(      strcmp(ON,  arg) == 0 ) *var = true;
    else if( strcmp(OFF, arg) == 0 ) *var = false;
    else mcprintf("!value is %s\n", *var ? ON : OFF, 0);
}

static struct {char *cmd; void (*cmd_action)(); Bool *var;} cmds[] =
{
    {"setrandom", toggle, &setrandom},
    {"echo", toggle, &echo_mode},
    {"verbose", toggle, &verbose},
    {"trace",  toggle, &trace_debug},
    {"source", toggle, &store_source},
    {"stack", toggle, &stack_dump},
    {"code",  toggle, &instruction_printing},
    {"gc", toggle, &gc_debug},
    {"rational", toggle, &do_rational},
    {0,0,0}
};



int     Bang(s)
    char   *s;
{
    char line[MAX_LINE], *replace;
    int i;

    i = 0;
    line[0] = '!';
    while(true) {
	command = skip_space(s + 1);
	com_end = find_space(command);
	arg = skip_space(com_end);
	*com_end = EOS;
	i++;
	if( (replace = in_alias(command)) == NULL ) break;

	/* if command in aliases, re-build command line */
	strncpy(line+1,replace,MAX_LINE-1);
	strcat(line," ");
	strncat(line, arg, MAX_LINE-1-strlen(line));
	line[MAX_LINE-1] = EOS;
	s = line;

	if( i > 10 )  {
	    mcprintf("!Alias loop in '%s'.  Ignored.\n", s,0);
	    return 0;
	}
    }


    if( strcmp(command, "alias") == 0 ) {
	char *name, *string;
	name = arg;
	arg  = find_space(arg);
	string = skip_space(arg);
	*arg = EOS;
	alias(name,string);
	return 0;

    } else if (strcmp(command, "pp") == 0) {
	char *id;
	char *file_name;
	IPtr  code;

	if( pp_out == 0 ) pp_out = stdout;

	if( *arg == EOS ) { /* change pp to stdout */
	    if( pp_out != stdout ) fclose(pp_out);
	    pp_out = stdout;
	    mcprintf("!pp to standard output\n",0,0);
	    return 0;
	}

	id = arg;
	arg_end = find_space(arg);
	file_name = skip_space(arg_end);
	*arg_end  = EOS;
	arg_end   = find_space(file_name);
	*arg_end  = EOS;
	    
	if( *file_name != EOS ) { /* change file */
	    if( pp_out != stdout ) fclose(pp_out);
	    pp_out = FOpen(file_name, "a");
	    if( pp_out == NULL ) {
		mcprintf("!can't append to %s\n", file_name,0);
		mcprintf("!pp to standard output\n",0,0);
		pp_out = stdout;
	    }
	}
	
	{ /* pp id to pp_out */
	    C_Ref i = find_id(id);
	    IPtr e = Get_Assoc(i);
	    IPtr val;

	    if( Kind(val = Var_Value(e)) != Closure ) {
		mcprintf("!'%s' is not a printable function\n", id,0);
		return 0;
	    }

	    for( code = Cl_SRC(val); code != Nil ; code = Next(code) ) {
		IPtr x = Info(code);
		if( Kind(x) == String )
		    mcfprintf(pp_out, "%s", Str_Value(x),0);
	    }
	    if( pp_out != stdout ) {
		fflush(pp_out);
		mcprintf("!'%s' appended\n", id,0);
	    }
	}
	
	return 0;

    } else if (strcmp(command, "credits") == 0) {
#ifdef Mac
   mcprintf("This version was compiled with Think-C (c) Symantec\n",0,0);
   mcprintf("Screen Editor based on the THINK C Class Library\n",0,0);
#endif

#ifdef TURBOC
   mcprintf("This version was compiled with Turbo-C (c) Borland\n",0,0);
   mcprintf("Screen Editor (c) Gary Levin 1989\n",0,0);
#endif
#ifndef SCREEN
mcprintf("The interactive line editor was derived from ILE\n",0,0);
mcprintf("  written by Robert C. Pendleton (c) 1988, Evans & Sutherland\n",
         0,0);
#endif

mcprintf("This program was developed while the author was a member of the\n",0,0);
mcprintf("Clarkson faculty, using the Z200 provided by the school.  West\n",0,0);
mcprintf("Publishing has provided a Macintosh for further refinement of the\n",0,0);
mcprintf("Mac version.  West has also provided support for the development of\n",0,0);
mcprintf("the PC and Mac graphics features.\n",0,0);
mcprintf("\n",0,0);
mcprintf("There are many people who have contributed to the ideas behind\n",0,0);
mcprintf("ISETL.  First and foremost is Ed Dubinsky, whose idea it was to use\n",0,0);
mcprintf("SETL for teaching Discrete Mathematics.  His dissatisfaction with\n",0,0);
mcprintf("the old implementation suggested this project.  Others include:\n",0,0);
mcprintf("Nancy Baxter, Don Muench, and that mysterious bunch that we just\n",0,0);
mcprintf("call IWEST.\n",0,0);
     return 0;
    } else if (strcmp(command, "clear") == 0) {
	mcprintf("!clear complete\n",0,0);
	Clear();
	inside = false;
	return CLEAR;
    } else if (strcmp(command, "quit") == 0) {
#ifdef Screen
#ifdef TURBOC
	if( direct_input ) do_exit(GOOD_EXIT);
	q_exit();
	return 0;
#endif
#ifdef Mac
	q_exit();
	return 0;
#endif
#else
	do_exit(GOOD_EXIT);
#endif
    } else if (strcmp(command, "oms") == 0) {
	Id_Dump(false,false);
	return 0;
    } else if (strcmp(command, "ids") == 0) {
	Id_Dump(true,false);
	return 0;
    } else if (strcmp(command, "locked") == 0) {
	Id_Dump(true,true);
	return 0;
    } else if (strcmp(command, "version") == 0) {
	mcprintf("%s\n", rev_info,0);
	return 0;

#ifndef Mac
    } else if (strcmp(command, "system") == 0) {

#ifdef Screen
	sys(arg);
#else
	system(arg);
#endif

	return 0;
#endif
    } else if (strcmp(command, "watch") == 0
	       || strcmp(command, "unwatch") == 0
	       || strcmp(command, "lock") == 0
	       || strcmp(command, "unlock") == 0 )
    {
        enum {watch,unwatch,lock,unlock} action;
	Bool    done = false;

        if( strcmp(command, "watch")   == 0) action = watch;
	if( strcmp(command, "unwatch") == 0) action = unwatch;
        if( strcmp(command, "lock")    == 0) action = lock;
	if( strcmp(command, "unlock")  == 0) action = unlock;

	while (!done) {
	    /* get arg_end */
	    arg_end = find_space(arg);
	    done = (*arg_end == EOS);
	    *arg_end = EOS;
	    if (*arg == EOS)
		break;

	    if (!good_id(arg)) {
		mcprintf("!'%s' not an id\n", arg,0);
	    } else {
                if( echo_mode || (!silent && !silent_running) )
		    mcprintf("!'%s' %sed\n", arg, command);
                switch(action) {
		    case watch:   Watch(arg);    break;
		    case unwatch: UnWatch(arg);  break;
		    case lock:    Lock(arg);     break;
		    case unlock:  UnLock(arg);   break;
                }
	    }
	    /* get arg */
	    arg = skip_space(arg_end + 1);
	}
	return 0;

    } else {			       /* single argument commands */
	arg_end = find_space(arg);
	*arg_end = EOS;
	if (strcmp(command, "allocate") == 0) {
	    long    maxgc_mem = 0;
	    char   *form = "%ld";

	    sscanf(arg, form, &maxgc_mem);
	    while (maxgc_mem > current_mem && Add_Segment()) {
		/* do nothing.  Add_Segment has done it. */
	    }
	    mcprintf("Current GC memory = %ld, Limit = %ld\n",
		   current_mem, max_mem);
	    return 0;
	} else if (strcmp(command, "memory") == 0) {
	    if (arg == arg_end) {
		mcprintf("Current GC memory = %ld, Limit = %ld\n",
		       current_mem, max_mem);
	    } else {
		long    maxgc_mem = 0;
		char   *form = "%ld";

		sscanf(arg, form, &maxgc_mem);
		if (maxgc_mem > current_mem)
		    max_mem = maxgc_mem;
		mcprintf("Current GC memory = %ld, New Limit = %ld\n",
		       current_mem, max_mem);
	    }
	    return 0;
	} else if (strcmp(command, "include") == 0) {
	    if (pushes != 0) {
		WARNING("Can't include during read...from...\n");
		return 0;
	    }
	    stack[top] = yyin;
	    ln_stack[top] = ++line_num;
	    last_read = EOS;
	    line_num = 1;
	    top++;
	    if (top == MAX_INCLUDE) {
		FATAL("Includes too deeply nested");
	    }
	    yyin = FOpen(arg, "r");
	    if (FNULL == yyin) {
		mcprintf("Cannot open '%s'\n", arg,0);
		--top;
		yyin = stack[top];
		line_num = --ln_stack[top];
		prompt(last_prompt);
	    } else {
		fn_stack[top][MAX_FN - 1] = '\0';
		strncpy(fn_stack[top], arg, MAX_FN - 1);
	    }
	    return 0;

	} else if (strcmp(command, "record") == 0
		   && *arg != EOS) {
	    if (record != NULL) {
		fclose(record);
		mcprintf("! Previous record closed\n",0,0);
	    }
	    record = FOpen(arg, "a");
	    mcprintf("! Recording on '%s'\n", arg,0);
	    return 0;
	} else if (strcmp(command, "record") == 0
		   && *arg == EOS) {
	    if (record != NULL) {
		fclose(record);
		record = NULL;
		mcprintf("! Recording off\n",0,0);
	    } else {
		mcprintf("! Recording stays off\n",0,0);
	    }
	    return 0;
	} else if (strcmp(command, "slow") == 0) {
	    single_step = 1;
	    return 0;
	} else if (strcmp(command, "fast") == 0) {
	    single_step = 0;
	    return 0;
	} else {
	    int i;
	    for( i = 0; cmds[i].cmd != NULL ; i++ ) {
		if( strcmp(command, cmds[i].cmd) == 0 ) {
		    (*(cmds[i].cmd_action))(cmds[i].var);
		    return 0;
		}
	    }
	    mcprintf("Unknown compiler directive \"%s\" ignored\n", s,0);
	    return 0;
	}
    }
}




#define MAX_ALIASES 50
int find_aliases(STR);

static char * names[MAX_ALIASES];
static char * strings[MAX_ALIASES];
static int aliases = 0;

static int find_alias(name) char *name; {
    int i;
    for( i = 0 ; i < aliases; i++ ){
	if( strcmp(name, names[i]) == 0 ) return i;
    }
    return i;
}

static void alias(name,string) char *name, *string; {
    int i = find_alias(name);
    if( i < aliases ) {
	mcprintf("!Duplicate alias '%s' replaces old definition\n", name,0);
	free(names[i]);
	free(strings[i]);
	names[i]   = str(name);
	strings[i] = str(string);
    } else if( aliases == MAX_ALIASES) {
	mcprintf("!Too many aliases\n", 0,0);
    } else {
	names[aliases]   = str(name);
	strings[aliases] = str(string);
	if( names[aliases] != NULL) aliases++;
    }
}

static char *in_alias(name) char *name; {
    int i = find_alias(name);
    if( i < aliases )   return strings[i];
    else		return NULL;
}

