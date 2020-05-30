#include "common.h"
/* $Header: op_io.c,v 3.1 90/04/25 16:23:50 gary Locked $ */
#include "instr.h"
#include "op.h"
#include "manager.h"
#include "micro.h"
#include "emitter.h"
/* Because these are ISETL operations, the arguments are on the ISETL
 * stack, not passed as C arguments.  Comments refer to the arguments
 * on the stack.  They are correctly accessed by the ENTER macros.
 * See op.h for an explanation of ENTER, LEAVE, R(X_Op).
 * See instr.h for further description of ISETL operations.
 */


/* (x,y) Prints each element of list x on f, followed by \n */
/* if y is not a file, give an error */
/* Nil represents stdout */
void    opPrint()
{
    C_File  f;

    NO_BIN_ENTER;

    if (Kind(R(Right_Op)) == File
	&& File_Value(R(Right_Op)) != FNULL
	&& File_Mode(R(Right_Op)) != 'r') {
	f = File_Value(R(Right_Op));
    } else if (R(Right_Op) == Nil) {
	f = stdout;
    } else {
	show("Can't write to: ", stdout);
	dbPrint(R(Right_Op), stdout);
	mcFlush(stdout);
	ERROR;
    }

    {
	IPtr     p;

	for (p = R(Left_Op); p != Nil; p = Next(p)) {
	    mcPrint(Info(p), f, true);
	    mcFlush(f);
	}
    }

    BIN_NO_RESULT;
}



/* (x,f)  Prints elements of list x on f.	 */
/* Elements of x come in pairs (value,format).	 */
/* if f is not a file, gives error		 */
/* Nil represents stdout			 */
void    opPrintF()
{
    C_File  f;

    NO_BIN_ENTER;

    if (Kind(R(Right_Op)) == File
	&& File_Value(R(Right_Op)) != FNULL
	&& File_Mode(R(Right_Op)) != 'r') {
	f = File_Value(R(Right_Op));
    } else if (R(Right_Op) == Nil) {
	f = stdout;
    } else {
	show("Can't write to: ", stdout);
	dbPrint(R(Right_Op), stdout);
	mcFlush(stdout);
	ERROR;
    }

    {
	IPtr     p;

	for (p = R(Left_Op); p != Nil; p = Next(Next(p))) {
	    mcPrintF(Info(p), Info(Next(p)), f);
	}
    }

    if (f == stdout)
	mcfflush(f);
    BIN_NO_RESULT;
}



/* (x,f) Reads objects from f assigns them */
/* to alternate elements of x, following elements are format  */
/* if f is not a file, give an error */
/* Nil represents current input */
void    opReadF()
{
    C_File  f;

    NO_BIN_ENTER;
    Reg     (item);
    Reg     (form);

    if (Kind(R(Right_Op)) == File
	&& File_Value(R(Right_Op)) != FNULL
	&& File_Mode(R(Right_Op)) == 'r') {
	f = File_Value(R(Right_Op));
    } else if (R(Right_Op) == Nil) {
	f = FNULL;
    } else {
	show("Can't read from: ", stdout);
	dbPrint(R(Right_Op), stdout);
	mcFlush(stdout);
	ERROR;
    }

    geof = false;

    R(Left_Op) = mcStart(Tuple_Value(R(Left_Op)));
    while (mcNext(&R(item), &R(Left_Op), true)) {
	int     c;

	mcNext(&R(form), &R(Left_Op), true);
	c = mcReadF(R(item), R(form), f,
		    f == FNULL ? EOS : File_Last(R(Right_Op)));
	if (f != FNULL)
	    File_Last(R(Right_Op)) = c;
    }
    if (geof && f != FNULL) {
	File_Eof(R(Right_Op)) = true;
    }
    geof = false;
    BIN_NO_RESULT;
}



/* (x,f) Reads objects from f assigns them */
/* to each element of x	 */
/* if f is not a file, give an error */
/* Nil represents current input */
void    opRead()
{
    C_File  f;

    NO_BIN_ENTER;
    Reg     (item);

    if (Kind(R(Right_Op)) == File
	&& File_Value(R(Right_Op)) != FNULL
	&& File_Mode(R(Right_Op)) == 'r') {
	f = File_Value(R(Right_Op));
    } else if (R(Right_Op) == Nil) {
	f = FNULL;
    } else {
	show("Can't read from: ", stdout);
	dbPrint(R(Right_Op), stdout);
	mcFlush(stdout);
	ERROR;
    }

    geof = false;
    R(Left_Op) = mcStart(Tuple_Value(R(Left_Op)));
    while (mcNext(&R(item), &R(Left_Op), true)) {
	int     c = mcRead(R(item), f,
			   f == FNULL ? EOS : File_Last(R(Right_Op)));

	if (f != FNULL)
	    File_Last(R(Right_Op)) = c;
    }
    if (geof && f != FNULL) {
	File_Eof(R(Right_Op)) = true;
    }
    geof = false;
    BIN_NO_RESULT;
}
