#include	"common.h"
/* $Header: symbol.c,v 3.1 90/04/25 16:24:31 gary Locked $ */
#include	"symbol.h"
#include	"manager.h"
#include    <stdlib.h>
#define Max_Id_Chars	3000


static char **ids = NULL;
static Bool *id_trace;
static Bool *id_locked;
static IPtr *global_val;
static char *id_strings;
static char *next_char;
static int known = 0;		       /* ids[1..known] known */
static C_Ref anon = 0;		       /* anon..-1 in use as anon vars */

void Watch(s)   char *s; {C_Ref i = find_id(s); id_trace[i] = true;}
void UnWatch(s) char *s; {C_Ref i = find_id(s); id_trace[i] = false;}
Bool Watched(i) C_Ref i; {if (0 < i && i <= known) return id_trace[i];
                          return false;}

void Lock(s)   char *s; {C_Ref i = find_id(s); id_locked[i] = true;}
void UnLock(s) char *s; {C_Ref i = find_id(s); id_locked[i] = false;}
Bool Locked(i) C_Ref i; {if (0 < i && i <= known) return id_locked[i];
                         return false;}
IPtr Global_Val(i) C_Ref i; {
    if (0 < i && i <= known) return global_val[i];
    else {
        IPtr e = Environment;
        while( Next(e) != Nil ) e = Next(e);
        Next(e) = New_Block(List,0);
        e = Next(e);
        Info(e) =  New_Block(Assoc, 0);
	Var_Index(Info(e)) = i;
	return Info(e);
    }
}

void Mark_Globals() {
    int i; 
    for( i=1; i<=known; i++ ) Mark_All(global_val[i]);
}

char   *Id_String(i)
    int     i;
{
    if (0 < i && i <= known)
	return ids[i];
    if (0 > i)
	return "Anonymous";
    return "Bad-id (internal error)";
}

#define SETUP(x,i)   x = calloc(i, sizeof*x)

C_Ref   find_id(p)
    char   *p;
{
    int     i;
    if( ids==NULL ) {
        SETUP(ids,Max_Vars);
        SETUP(id_trace,Max_Vars);
        SETUP(id_locked,Max_Vars);
        SETUP(id_strings,Max_Id_Chars);
        SETUP(global_val,Max_Vars);
        next_char = id_strings;
    }
    
    for (i = 1; i <= known; i++) {
	if (strcmp(p, ids[i]) == 0)
	    return i;
    }
    known++;
    ids[known] = next_char;
    id_trace[known] = false;
    id_locked[known] = false;
    global_val[known] =  New_Block(Assoc, 0);
	Var_Index(global_val[known]) = i;

    strcpy(ids[known], p);
    next_char += strlen(p) + 1;
    if (next_char >= id_strings + Max_Id_Chars
	|| known >= Max_Vars)
	FATAL("Too many variables");
    return known;
}

C_Ref   Mk_Var()
{
    anon--;
    if (anon < Anons)
	Anons = anon;
    return anon;
}

void    Free_Var()
{
    anon++;
}

void    Print_Id(i)
    int     i;
{
    if (i < 0) {
	mcprintf("Anon(%d)", -i,0);
    } else {
	mcprintf("Var(%d) '%s'", i, ids[i]);
    }
}

void    Id_Dump(defined,locked)
    Bool    defined,locked;
{
    IPtr     p;
    int     i;
    int     g;
    C_Ref   indx;

    for (p = Environment, i = 0; p != Nil; p = Next(p)) {
	indx = Var_Index(Info(p));
	if (indx > 0 && (Kind(Var_Value(Info(p))) != Undefined) == defined
	             && Locked(indx)==locked) {
	    i++;
	    mcprintf("%s%c", ids[indx],
		   (i % 5 == 0 ? '\n' : '\t'));
	}
    }
    for (g=1; g<=known; g++) {
	indx = Var_Index(global_val[g]);
	if (indx > 0 && (Kind(Var_Value(global_val[g])) != Undefined) == defined
	             && Locked(indx)==locked) {
	    i++;
	    mcprintf("%s%c", ids[indx],
		   (i % 5 == 0 ? '\n' : '\t'));
	}
    }
    mcprintf("\n!dump complete\n",0,0);
}

IPtr Get_Assoc(i) C_Ref i; {
    IPtr e;

    TRACE_VARS("Push", i);
    /* Find e where Info(e) = <i,i-value> */
    e = Environment;
    while (e != Nil && Var_Index(Info(e)) != i) {
	e = Next(e);
    }
    if (e==Nil) {
	return Global_Val(i);
    } else
	return Info(e);
}

