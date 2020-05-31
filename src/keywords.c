#include "common.h"
/* $Header: keywords.c,v 3.1 90/04/25 16:22:58 gary Locked $ */
#include "parser.h"

struct Binding {
    char   *key;
    int     token;
};


#define TABLE_SIZE	103
#define MAX_KEY		5
/* MAX_KEY is chosen to avoid shifting into the sign bit */

static struct Binding Table[TABLE_SIZE];

/* staic forward declarations */
void    Key_Bind(STR X INT);
int     hash(STR);

static int hash(s)
    char   *s;
{
    int     h,
            i;

    i = h = 0;
    for (; *s && i < MAX_KEY; s++, i++) {
	h <<= 1;
	h += (int) *s;
    }
    return h % TABLE_SIZE;
}

int     locate(STR);

static int locate(s)
    char   *s;
{
    int     h;

    h = hash(s);
    while (Table[h].key != CNULL && strcmp(Table[h].key, s) != 0) {
	h = (h + 1) % TABLE_SIZE;
    }
    return h;
}

static void Key_Bind(s, t)
    char   *s;
    int     t;
{
    int     h;

    h = locate(s);
    assert(Table[h].key == CNULL);
    Table[h].key = s;
    Table[h].token = t;
}


/* Initialize list of keywords */
void    Init_Key()
{
    int     i;

    for (i = 0; i < TABLE_SIZE; i++) {
	Table[i].key = CNULL;
    }
    Key_Bind("and", T_And);
    Key_Bind("choose", T_Choose);
    Key_Bind("div", T_Div);
    Key_Bind("do", T_Do);
    Key_Bind("else", T_Else);
    Key_Bind("elseif", T_Elseif);
    Key_Bind("end", T_End);
    Key_Bind("exists", T_Exists);
    Key_Bind("false", T_False);
    Key_Bind("for", T_For);
    Key_Bind("forall", T_Forall);
    Key_Bind("from", T_From);
    Key_Bind("fromb", T_Fromb);
    Key_Bind("frome", T_Frome);
    Key_Bind("func", T_Func);
    Key_Bind("if", T_If);
    Key_Bind("iff", T_Iff);
    Key_Bind("impl", T_Impl);
    Key_Bind("in", T_In);
    Key_Bind("inter", T_Times);
    Key_Bind("less", T_Less);
    Key_Bind("local", T_Local);
    Key_Bind("mod", T_Mod);
    Key_Bind("newat", T_Newat);
    Key_Bind("not", T_Not);
    Key_Bind("notin", T_Notin);
    Key_Bind("of", T_Of);
    Key_Bind("om", T_Om);
    Key_Bind("OM", T_Om);
    Key_Bind("opt", T_Opt);
    Key_Bind("or", T_Or);
    Key_Bind("print", T_Print);
    Key_Bind("printf", T_PrintF);
    Key_Bind("proc", T_Proc);
    Key_Bind("program", T_Prog);
    Key_Bind("read", T_Read);
    Key_Bind("readf", T_ReadF);
    Key_Bind("return", T_Return);
    Key_Bind("subset", T_Subset);
    Key_Bind("take", T_Take);
    Key_Bind("then", T_Then);
    Key_Bind("to", T_To);
    Key_Bind("true", T_True);
    Key_Bind("union", T_Plus);
    Key_Bind("value", T_Value);
    Key_Bind("where", T_Where);
    Key_Bind("while", T_While);
    Key_Bind("with", T_With);
    Key_Bind("write", T_Write);
    Key_Bind("writeln", T_Writeln);
}


/* Return Token for s if s is a keyword, 0 ow */
int     Key(STR);
int     Key(s)
    char   *s;
{
    int     h;

    h = locate(s);
    if (Table[h].key == CNULL)
	return 0;
    else
	return Table[h].token;
}
