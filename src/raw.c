#include "machine.h"
#include <stdlib.h>

extern int direct_input;
  /* make sure type here agrees with definition of Bool */

#ifdef Screen
#ifdef TURBOC
#include <conio.h>
#endif
#endif

#ifdef Raw

#ifdef SYS5
#	include <termio.h>
#else
#ifdef Mac
#   include <console.h>
#else
#	include <curses.h>
#endif
#endif

#ifdef SYS5
static struct termio oldterm,
        newterm;

static void savetty()
{
    ioctl(0, TCGETA, &oldterm);
    ioctl(0, TCGETA, &newterm);
}

static void resetty()
{
    ioctl(0, TCSETAW, &oldterm);
}


/* turn off canonical cooking */
static void cbreak()
{
    newterm.c_lflag &= ~(ICANON);
    newterm.c_cc[VEOF] = 1;
    newterm.c_cc[VEOL] = 0;

    ioctl(0, TCSETAW, &newterm);
}

/* turn off echo */
static void noecho()
{
    newterm.c_lflag &= ~(ECHO);

    ioctl(0, TCSETAW, &newterm);
}
/* SYS5 */
#endif

#ifdef Mac
static void savetty() {}
static void cbreak()  {}
static void noecho()  {csetmode(C_RAW,stdin);}
static void resetty() {}

int getch(){ int c;
    while( (c=getchar())==EOF ) ;
    return c;
}
#endif


void    start_raw()
{
    savetty();

#ifdef CRmode
    crmode();

#else
    cbreak();
#endif

    noecho();
}

#endif


void    do_exit(i)
    int     i;
{
#ifdef Raw
    if( !direct_input )    resetty();
#endif

#ifdef Screen
#ifdef TURBOC
    if( !direct_input ) {
       textmode(LASTMODE);
       clrscr();
    }
#endif
#endif
    exit(i);
}

