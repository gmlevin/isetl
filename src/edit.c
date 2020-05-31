#include "common.h"
/* $Header: edit.c,v 3.2 90/04/25 16:22:23 gary Locked $ */
#include "parser.h"
#include "ast.h"
#include "manager.h"
#include "scanner.h"
#include "micro.h"
#include <ctype.h>

/* maintain last MAXERRBUFF chars read for error messages */
#define MAXERRBUFF 200
static char errbuff[MAXERRBUFF];


static int in_err = 0;
static Bool errfull = false;


/* Print the last lines of input */
void    Show_Error()
{
    int     i;

    mcputc('\n');
    if (errfull) {
	for (i = in_err; i < MAXERRBUFF; i++) {
	    mcprintf("%c", errbuff[i],0);
	}
    }
    for (i = 0; i < in_err; i++) {
	mcprintf("%c", errbuff[i],0);
    }
    mcputc('\n');
}


/* Clear error buffer */
void    Clear()
{
    in_err = 0;
    errfull = false;
    func_level = 0;
    Func_List  = Nil;
}


Bool    last_nl = false;	       /* was last char echoed a newline */
char    last_caught = EOS;	       /* last char returned from catch */

/* Print newline if echoing input */
void    New_Line(NONE);
void    New_Line()
{
    if (!last_nl && echo_mode)
	mcputc('\n');
    last_nl = true;
}

void    reset_line()
{
    while( last_caught != '\n' && last_caught != (char)EOF)
          catch(); /* keep reading */
}


int immediate_char(){
    int c;
    if (direct_input) {
	c = getc(yyin);
    } else if (yyin == CONin || yyin == stdin) {
#ifdef Screen
	c = Egetc();
#else
	c = ile_getc();
#endif
    } else {
	c = getc(yyin);
    }
    if (c == '\r')
	c = '\n';
    return c;
}


/*
 * supplies characters for input.
 * "!clear", or yyin
 *
 * also buffers input chars
 */

static char catch_line[Max_Line+1];
static int  c_next=0, c_len=0;
int     catch()
{
    int     c;

    if( reading && pushes>0 ) { /* read directly from file */
	c = immediate_char();
    } else { /* read from buffer */
	/* keep buffer full */
	while( c_next >= c_len ) {
	    c_next = 0;
	    do {
		c = immediate_char();
		catch_line[c_next++] = c;

	    } while( c != EOF && c != '\n' && c_next < Max_Line );

	    c_len = c_next;
	    catch_line[c_len] = EOS;
	    c_next = 0;
	    if(echo_mode) {
		char * p = catch_line;
		for( ; *p != EOS ; p++ ) {
		    if( *p != (char)EOF ) mcputc(*p);
		}
	    }
	    last_nl = (c == '\n');
	}
	/* c = next char */
	c = catch_line[c_next++];
	if( c==(char)EOF ) c = EOF;
    }

    if (c != EOF) {
	/* buffer c */

	if (yyin == stdin && record != NULL) {
	    fputc(c, record);
	    if (c == '\n')
		fflush(record);
	}
	errbuff[in_err] = c;
	in_err = (in_err + 1) % MAXERRBUFF;
	if (in_err == 0)
	    errfull = true;
    }

    if( c == '\n' ) Source_Flush();

    last_caught = c;
    return c;
}




/* Indicate that you are done with the current source line
 * Link in all the statements that started on this line
 */
void Source_Flush()
{
    ENTER;
    Reg(line);
    AST_Ptr t;

    if (store_source) {
	R(line) = New_String(catch_line);
    } else {
	R(line) = ANULL;
    }
    if( Func_List != Nil ) {
	assert(Kind(Func_Tail)==List);
	Next(Func_Tail) = mcCons(R(line), Nil);
	Func_Tail = Next(Func_Tail);
    }

    while (Source_List != ANULL) {
	t = Source_List;
	Source_List = ast_1(Source_List);
	ast_1(t) = R(line);
    }

    LEAVE;
}
