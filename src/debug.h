
/* $Header: debug.h,v 3.1 90/04/25 16:24:43 gary Locked $ */

#ifdef Mac
extern void panic(short);
#endif

extern char rev_info[];

#ifdef TORTURE
    extern Bool torture;
#endif

extern Bool need_clear;
extern Bool verbose;
extern Bool store_source;
extern Bool stack_dump;
extern IPtr Last_Line;
#ifdef TURBOC
    extern Bool still_running;   /* used to flash the still running window */
#endif

extern void Show_Error(NONE);
extern void Show_Line(NONE);
extern void dbPrint(PTR X C_FILE);
extern char *Id_String(C_REF);
extern Bool Watched(INT);

extern void BAD_ARGS(STR X PTR X PTR);
extern void BAD_VAL(STR X PTR);
extern void BAD_REF(STR X PTR);
extern void BAD_SLICE(STR X PTR X PTR X PTR);
extern void BAD_SMAP(STR X PTR X PTR);
extern void BAD_MMAP(STR X PTR X PTR);
extern void BAD_MAP(STR X PTR X PTR);
extern void BAD_FORMAT(PTR X PTR);

#define		GC_DEBUG	if( gc_debug ) mcprintf

#define MSG(t,x)	{Show_Line(); mcprintf("\n!%s: %s\n", t,x);}
#define SYNTAX(x)	MSG("Syntax", x);
#define WARNING(x)	MSG("Warning", x);
#define RT_ERROR(x)	{MSG("Error", x); ERROR;}

#ifdef Screen
#ifdef TURBOC
void save_buff(NONE);
#define FATAL(x)	{MSG("Fatal", x); save_buff(); do_exit(BAD_EXIT);}
#endif
#ifdef Mac
#define FATAL(x)	{MSG("Fatal", x); panic(262);}
#endif
#else
#define FATAL(x)	{MSG("Fatal", x); do_exit(BAD_EXIT);}
#endif

#define ERROR		longjmp(err_ret,1)

/* Print p in either long or short form, depending on verbose */
#define dbPrint(p, f)    mcPrint(p, f, verbose)

#define NA(x)	mcprintf("%s -- Not Available\n", x)

#define TRACE_VARS(msg,i) \
	if( trace_debug ) { \
	    mcprintf( "%s -- ", msg);	\
	    Print_Id(i);	\
	    mcputc('\n'); }
