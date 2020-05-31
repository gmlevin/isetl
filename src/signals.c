#include "common.h"
#include "scanner.h"
/* $Header: signals.c,v 3.1 90/04/25 16:24:28 gary Locked $ */

#ifdef Signals
#include <signal.h>


static Bool occurred = false;

#ifdef TURBOC
#include <dos.h>
#endif

#define INT_TYPE void
#define INT_RET


INT_TYPE on_int(x) int x;
{
    occurred = false;
    Init_Signals();

#ifdef GOULD
    sleep(1);			       /* this gives the interrupt handler
				        * time to clean up.  In
				        * particular, the Gould system
				        * trashes some of the next few
				        * lines of output if there is no
				        * delay here */
#endif

    mcprintf("\nINTERRUPT\n");
    Clear();

    reset_line();
    inside = false;
    prompt(last_prompt);

    ERROR;
    return INT_RET;
}


INT_TYPE sig_ign(x) int x;
{
    occurred = true;
    Hold_Signals();
    return INT_RET;
}


#ifdef SIGFPE
    INT_TYPE on_fpe(x,y) int x,y;
{
    Init_Signals();
    RT_ERROR("Floating point error");
    return INT_RET;
}

#endif

/* initialize signal catching routines */
void    Init_Signals()
{

    signal(SIGINT, on_int);

#ifdef SIGFPE
    signal(SIGFPE, on_fpe);
#endif

    if (occurred)
	on_int(0);
}


/* suppress signal catching routines */
void    Hold_Signals()
{

    signal(SIGINT, sig_ign);

#ifdef SIGFPE
    signal(SIGFPE, sig_ign);
#endif

}
#endif

#ifdef TURBOC
int matherr(e) struct exception *e; {
    RT_ERROR("Arithmetic error");
}
#endif
