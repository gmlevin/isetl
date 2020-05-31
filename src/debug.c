#include "common.h"
#include "micro.h"

Bool    verbose = false;	       /* controls type vs value printing
				        * for debug info */

/* Print Last_Line (last line read or line of last stmt executed) */
void    Show_Line()
{
    if (Last_Line != Nil)
	mcprintf("\n*** %s", Str_Value(Last_Line),0);
}

/* General Error messages */

void    BAD_FORMAT(f, x)
    IPtr     f,
            x;
{
    MSG("Error", "Bad format:");
    last_nl = true;
    dbPrint(f, stdout);
    show(" for ", stdout);
    dbPrint(x, stdout);
    mcFlush(stdout);
    ERROR;
}

void    BAD_ARGS(s, left, right)
    char   *s;
    IPtr     left,
            right;
{
    MSG("Error", "Bad arguments in:");
    last_nl = true;
    if (left != NULL)
	dbPrint(left, stdout);
    show(" ", stdout);
    show(s, stdout);
    show(" ", stdout);
    dbPrint(right, stdout);
    mcFlush(stdout);
    ERROR;
}

void    BAD_VAL(s, val)
    char   *s;
    IPtr     val;
{
    MSG("Error", s);
    last_nl = true;
    dbPrint(val, stdout);
    mcFlush(stdout);
    ERROR;
}

void    BAD_REF(s, ref)
    char   *s;
    IPtr     ref;
{
    MSG("Error -- Bad mapping", s != NULL ? s : "");
    last_nl = true;

    dbPrint(ref, stdout);
    mcFlush(stdout);
    ERROR;
}

void    BAD_SLICE(s, fn, low, high)
    char   *s;
    IPtr     fn,
            low,
            high;
{
    MSG("Error -- Bad slice", s != NULL ? s : "");
    last_nl = true;

    dbPrint(fn, stdout);
    show("(", stdout);
    dbPrint(low, stdout);
    show("..", stdout);
    dbPrint(high, stdout);
    show(")", stdout);
    mcFlush(stdout);
    ERROR;
}

void    BAD_SMAP(s, fn, idx)
    char   *s;
    IPtr     fn,
            idx;
{
    MSG("Error -- Bad mapping", s != NULL ? s : "");
    last_nl = true;

    dbPrint(fn, stdout);
    show("(", stdout);
    dbPrint(idx, stdout);
    show(")", stdout);
    mcFlush(stdout);
    ERROR;
}

void    BAD_MMAP(s, fn, idx)
    char   *s;
    IPtr     fn,
            idx;
{
    MSG("Error -- Bad mapping", s != NULL ? s : "");
    last_nl = true;

    dbPrint(fn, stdout);
    show("{", stdout);
    dbPrint(idx, stdout);
    show("}", stdout);
    mcFlush(stdout);
    ERROR;
}

void    BAD_MAP(s, fn, select)
    char   *s;
    IPtr     fn,
            select;
{
    MSG("Error -- Bad mapping", s != NULL ? s : "");
    last_nl = true;

    dbPrint(fn, stdout);
    show(Sel_Is(select) == Sel_MMap ? "{" : "(", stdout);
    dbPrint(At_Index(select), stdout);
    if (Sel_Is(select) == Sel_Slice) {
	show("..", stdout);
	dbPrint(To_Index(select), stdout);
    }
    show(Sel_Is(select) == Sel_MMap ? "}" : ")", stdout);
    mcFlush(stdout);
    ERROR;
}


/* Not used in program.  Available for invocation by debuggers for
 * printing of ISETL objects
 */
void    dbxprint(PTR);
void    dbxprint(x)
    IPtr     x;
{
    mcPrint(x, stdout, verbose);
    mcFlush(stdout);
}
