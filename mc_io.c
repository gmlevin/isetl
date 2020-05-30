#include "common.h"
#include "sort.h"
/* $Header: mc_io.c,v 3.2 90/04/25 16:23:13 gary Locked $ */
#include "micro.h"
#include "manager.h"
#include <ctype.h>

/* static forward declaration */
void    Seq_Print(PTR X C_FILE X BOOL X BOOL);
void    mcPrintOrdered(PTR X C_FILE X BOOL);

void    New_Line(NONE);		       /* edit.c */

/* cannot set Max_Out_Line greater than Abs_Max_Out_Line */
#define Abs_Max_Out_Line	200

int     Max_Out_Line = 75;

static int depth = 0;		       /* nesting depth of brackets and
				        * braces */
static int out_col = 0;		       /* last column printed in */
static char buffer[Abs_Max_Out_Line + 10]; /* buffered output */

/* +10 give room for `" +"' for splitting strings, plus slack */
/* slack needed for "\000" type constants		      */

int     r_prec = 3;		       /* number of decimal places for
				        * print of reals
					* < 0 ==> use %e */

/* add string s to buffer, print to f if need to break line */
void    show(s, f)
    char   *s;
    C_File  f;
{
    int     i,
            j;
    int     split,		       /* position of rightmost blank at
				        * smallest depth seen in buffer */
            min_depth,		       /* smallest depth seen in buffer */
            temp_depth,		       /* current depth */
            quoted;		       /* in quote, brackets don't count */

    while (*s) {		       /* there is a character to add to
				        * buffer */

	while (out_col >= Max_Out_Line) {  /* overflow of line */

	    if (depth > Max_Out_Line * 0.75) {
		mcFlush(f);
		RT_ERROR("Nesting too deep for pretty printer.");
	    }
	    /* find split: 0<=split<out_col: buffer[split]==' ' */
	    /* if there is a blank at depth, split there	 */
	    /* otherwise choose the last, least nested ' '	 */
	    do {
		temp_depth = depth;
		min_depth = Abs_Max_Out_Line;
		split = Abs_Max_Out_Line;
		quoted = false;
		for (i = depth; i < out_col; i++) {
		    if (quoted) {
			if (buffer[i] == Quote)
			    quoted = false;
		    } else
			switch (buffer[i]) {
			  case Set_Open:
			  case Tuple_Open:
			    temp_depth++;
			    break;
			  case Set_Close:
			  case Tuple_Close:
			    temp_depth--;
			    break;
			  case Quote:
			    quoted = true;
			    break;
			  case ' ':
			    if (temp_depth <= min_depth) {
				min_depth = temp_depth;
				split = i;
			    }
			    if (temp_depth < depth)
				i = out_col;
			    break;
			}
		}

		if (split == Abs_Max_Out_Line && !quoted) {
		    buffer[out_col++] = '\\';
		    buffer[out_col++] = ' ';
		} else if (split == Abs_Max_Out_Line) {
		    buffer[out_col++] = '"';
		    buffer[out_col++] = ' ';
		    buffer[out_col++] = '+';
		    buffer[out_col++] = '"';
		}
	    } while (split == Abs_Max_Out_Line);

	    /* print buffer[0..split-1] */
	    New_Line();
	    quoted = false;
	    for (i = 0; i < split; i++) {
		mcfputc(buffer[i], f);
		if (quoted) {
		    if (buffer[i] == Quote)
			quoted = false;
		} else
		    switch (buffer[i]) {
		      case Set_Open:
		      case Tuple_Open:
			depth++;
			break;

		      case Set_Close:
		      case Tuple_Close:
			depth--;
			break;

		      case Quote:
			quoted = true;
			break;
		    }
	    }
	    mcfputc('\n', f);

	    /* shift buffer[split+1..out_col-1] to buffer[0..] */
	    /* with depth indent */
	    for (i = 0; i < depth; i++) {
		buffer[i] = ' ';
	    }
	    for (i = split + 1, j = depth; i < out_col; i++, j++) {
		buffer[j] = buffer[i];
	    }
	    out_col = j;
	}

	/* move a character from s to buffer */
	if (*s == '\\') {	       /* move all */
	    int     i = strlen(s);

	    strcpy(&buffer[out_col], s);
	    out_col += i;
	    s += i;
	} else {
	    buffer[out_col] = *s;
	    out_col++;
	    s++;
	}
	assert(out_col <= Abs_Max_Out_Line + 10);
    }
}

/*  Write the rest of buffer to f */
void    mcFlush(f)
    C_File  f;
{
    int     i;

    New_Line();
    for (i = 0; i < out_col; i++) {
	mcfputc(buffer[i], f);
    }
    mcfputc(';', f);
    mcfputc('\n', f);
    out_col = depth = 0;
    mcfflush(f);
}

static void Seq_Print(p, f, ordered, verbose)
    IPtr     p;
    C_File  f;
    Bool    ordered,
            verbose;
{
    ENTER;
    Reg     (seq);
    Reg     (item);
    Bool    first = true;

    R(seq) = mcStart(p);
    while (mcNext(&R(item), &R(seq), ordered)) {
	if (!first) {
	    show(", ", f);
	}
	first = false;
	mcPrint(R(item), f, verbose);
    }
    LEAVE;
}

/* (p,f,verbose): Print p on f.
 * If verbose, print value.
 * If not verbose, just print type representation.
 */
void    mcPrint(p, f, verbose)
    IPtr     p;
    C_File  f;
    Bool    verbose;
{
    char    temp[80];

    switch (Kind(p)) {
      case Tree:
	if (!verbose)
	    show("!Tree!", f);
	else if (p == NilT) {
	    show("@", f);
	} else {
	    show("[", f);
	    mcPrint(Left(p), f, verbose);
	    show(" ", f);
	    mcPrint(Right(p), f, verbose);
	    show("]", f);
	} break;
      case HTree:
	if (!verbose)
	    show("!HTree!", f);
	else if (p == NilHT) {
	    show("@", f);
	} else {
	    sprintf(temp, "*%d,%ld*", HMax(p), HLen(p));
	    show(temp, f);
	    show("[", f);
	    mcPrint(HLeft(p), f, verbose);
	    show(" ", f);
	    mcPrint(HRight(p), f, verbose);
	    show("]", f);
	} break;

      case File:
	if (File_Value(p) != FNULL) {
	    sprintf(temp, "!File(%c:%lx)!", File_Mode(p), File_Value(p));
	    show(temp, f);
	} else {
	    show("!Closed File!", f);
	}
	break;

      case Predef:
	sprintf(temp, "!predef(%s)!", Pd_name(p));
	show(temp, f);
	break;

      case Closure:
	if (Cl_Override(p) != Nil) {
	    if (verbose) {
		show("{", f);
		Seq_Print(Cl_Override(p), f, false, verbose);
		show(" then ", f);
	    } else {
		show("{Modified:", f);
	    }
	}
	sprintf(temp, "!func(%d)!", Cl_Cnt(p));
	show(temp, f);
	if (Cl_Override(p) != Nil) {
	    show("}", f);
	}
	break;

      case Atom:
	sprintf(temp, "!%d!", Atom_Value(p));
	show(temp, f);
	break;

      case Integer:
	sprintf(temp, "%d", Int_Value(p));
	show(temp, f);
	break;

      case Bignum:
	mpPrint(p, f, true, 0, 0);
	break;

      case Rational:
	mpPrint(Num(p),f,true,0,0);
	show("/", f);
	mpPrint(Den(p),f,true,0,0);
	break;

      case Real:{
	    C_Real  r = Real_Value(p);
	    C_Real  a = r >= 0 ? r : -r;
	    int     prec = r_prec >= 0 ? r_prec : -r_prec;

	    if (r_prec >= 0
		&& ((1000000L > a && a >= 0.001) || a == 0.0)) {
		sprintf(temp, "%.*f", prec, r);
	    } else {
		sprintf(temp, "%.*e", prec, r);
	    }
	    show(temp, f);
	} break;

      case String:
	show("\"", f);
	{
	    char   *s = Str_Value(p);
	    char    mini[2];

	    mini[1] = EOS;
	    for (; *s; s++) {
		if (((unsigned)(*s)<=0177) && isprint(*s)) {
		    switch (*s) {
		      case '"':
			show("\\q", f);
			break;
		      case '\\':
			show("\\\\", f);
			break;
		      default:
			mini[0] = *s;
			show(mini, f);
		    }
		} else {
		    switch (*s) {
		      case '\n':
			show("\\n", f);
			break;
		      case '\t':
			show("\\t", f);
			break;
		      case '\f':
			show("\\f", f);
			break;
		      case '\b':
			show("\\b", f);
			break;
		      case '\r':
			show("\\r", f);
			break;
		      default:
			sprintf(temp, "\\%03o",
				0377 & *s);
			show(temp, f);
		    }
		}
	    }
	}
	show("\"", f);
	break;
	
      case SArray: case TArray: {
	  int i;
	  if( Kind(p)==SArray ) 	show("{", f);
	  else				show("[", f);

	  if( Array_Len(p) > 0 ) {
	      mcPrint(Array_Data(p)[0],f,verbose);
	      for(i=1; i<Array_Len(p); i++) {
		  show(", ",f);
		  mcPrint(Array_Data(p)[i],f,verbose);
	      }
	  }

	  if( Kind(p)==SArray ) 	show("}", f);
	  else				show("]", f);
      }  break;

      case List:
      case Set:
      case Tuple:{
	    Bool    is_set = Kind(p) == Set;
	    Bool    is_tup = Kind(p) == Tuple;
	    
	    if( is_set  && !setrandom )  {
		mcPrintOrdered(p,f,verbose);
		break;
	    }

	    /* no brackets around a list */

	    if (is_set) {
		p = Set_Value(p);
		show("{", f);
	    } else if (is_tup) {
		if( Tuple_Origin(p) != def_orig ) {
		    sprintf(temp, "%ld@", Tuple_Origin(p));
		    show(temp,f);
		}
		p = Tuple_Value(p);
		show("[", f);
	    }
	    if (verbose) {
		Seq_Print(p, f, !(is_set&&setrandom), verbose);
	    } else {
		if (is_set)
		    show("!Set!", f);
		else if (is_tup)
		    show("!Tuple!", f);
		else
		    show("!List!", f);
	    }

	    if (is_set)
		show("}", f);
	    else if (is_tup)
		show("]", f);
	}
	break;

      case Boolean:
	if (Bool_Value(p))
	    show("true", f);
	else
	    show("false", f);
	break;

      case Undefined:
	show("OM", f);
	break;

      case Ref:
	sprintf(temp, "%s",
		Id_String(Var_Index(Var_Assoc(p))));
	show(temp, f);
	{
	    Reg     (sels);

	    R(sels) = Sel_List(p);
	    while (R(sels) != Nil) {
		switch (Sel_Is(R(sels))) {
		  case Sel_SMap:
		    show("(", f);
		    dbPrint(At_Index(R(sels)), f);
		    show(")", f);
		    break;
		  case Sel_MMap:
		    show("{", f);
		    dbPrint(At_Index(R(sels)), f);
		    show("}", f);
		    break;
		  case Sel_Slice:
		    show("(", f);
		    dbPrint(At_Index(R(sels)), f);
		    show("..", f);
		    dbPrint(To_Index(R(sels)), f);
		    show(")", f);
		    break;
		  default:
		    assert(0);
		}
		R(sels) = Sel_Next(R(sels));
	    }
	}
	break;


      default:
	mcprintf("type = %d", Kind(p), 0);
	mcFlush(f);
	RT_ERROR("Bad arg to mcPrint");
    }
}



int    mcSetMaxLine(i)
    int     i;
{   int old;
    
    if (i > Abs_Max_Out_Line) {
	RT_ERROR("Max line too long");
    }
    if (i < 10) {
	RT_ERROR("Max line too short");
    }
    old = Max_Out_Line;
    Max_Out_Line = i;
    return old;
}



/* Print x on f using format */
void    mcPrintF(x, format, f)
    IPtr     x,
            format;
    C_File  f;
{
    ENTER;
    switch (Kind(x)) {
      case Atom:
      case Bignum:
      case Boolean:
      case Closure:
      case File:
      case Integer:
      case Predef:
      case Rational:
      case Real:
      case String:
      case Undefined:{
	    int     width,	       /* minimum cols used */
	            prec;	       /* places for Real. line break for
				        * Bignum */
	    C_Real  r;
	    Bool    alt;	       /* true ==> scientific, false ==>
				        * decimal */
	    char    temp[110];

	    /* set default values for width, prec, alt */
	    width = 10;
	    prec = 5;
	    alt = false;
	    switch (Kind(x)) {
	      case String:
		width = 0;
                prec  = Max_Out_Line;
		break;
	      case Rational:
	      case Bignum:
		prec = 50;
		break;
	      case Real:
		width = 20;
		break;
	    }

	    /* set user specified values for width, prec, alt */
	    switch (Kind(format)) {
	      case Undefined:
		break;

	      case Integer:
		width = Int_Value(format);
		if (width < 0) {
		    alt = true;
		    width = -width;
		}
		break;

	      case Real:
		r = Real_Value(format);
		if (r < 0) {
		    alt = true;
		    r = -r;
		}
		width = r;
		prec = (r - width + 0.005) * 100;
		break;
	      default:
		BAD_FORMAT(format, x);
	    }

	    temp[0] = EOS;
	    switch (Kind(x)) {
	      case Atom:
		sprintf(temp, "!%d!", Atom_Value(x));
		mcfprintf(f, "%*s", width, temp);
		break;

	      case Boolean:
		mcfprintf(f, "%*s", width,
			  Bool_Value(x) ? "true" : "false");
		break;

	      case File:
		if (File_Value(x) != FNULL) {
		    sprintf(temp, "!File(%c:%lx)!", File_Mode(x),
			    File_Value(x));
		} else {
		    sprintf(temp, "%s", "!Closed File!");
		}
		mcfprintf(f, "%*s", width, temp);
		break;

	      case Integer:
		mcfprintf(f, "%*d", width, Int_Value(x));
		break;

	      case Predef:
		sprintf(temp, "!predef(%s)!", Pd_name(x));
		mcfprintf(f, "%*s", width, temp);
		break;

	      case Rational:
		mpPrint(Num(x),f,false, width, prec);
		mcfprintf(f, "/", 0, 0);
		mpPrint(Den(x),f,false, width, prec);
		break;

	      case Real:
		if (alt) {
		    sprintf(temp, "%*.*e", width, prec, Real_Value(x));
		    mcfprintf(f, "%s", temp, 0);
		} else {
		    sprintf(temp, "%*.*f", width, prec, Real_Value(x));
		    mcfprintf(f, "%s", temp, 0);
		}
		break;

	      case String: {
                char text[Abs_Max_Out_Line+10];
                char * p = Str_Value(x);
                int len  = strlen(p);
                while( len > prec ) {
                    strncpy(text, p, prec);
		    text[prec]   = '\\';
                    text[prec+1] = EOS;
		    p   += prec;
		    len -= prec;
		    mcfprintf(f, "%*s\n", width, text);
                }
		mcfprintf(f, "%*s", width, p);
	      } break;

	      case Undefined:
		mcfprintf(f, "%*s", width, "OM");
		break;

	      case Bignum:
		mpPrint(x, f, false, width, prec);
		break;

	      case Closure:
		sprintf(temp, "!%sfunc(%d)!",
			(Cl_Override(x) != Nil ? "modified:" : ""),
			Cl_Cnt(x));
		mcfprintf(f, "%*s", width, temp);
		break;
	      default:
		assert(0);
	    }
	} break;

      case Tuple:
      case Set:{
	    Bool    ordered = (Kind(x) == Tuple) || !setrandom;
	    Reg     (seq);
	    Reg     (item);

	    /* print each element of x, using format */
	    R(seq) = mcStart((Kind(x) == Set)
			     ? Set_Value(x)
			     : Tuple_Value(x));

	    switch (Kind(format)) {
	      case Tuple:{	       /* cycle through format items */
		    Bool    printed = false;
		    Bool    fetch = true;
		    Reg     (form_list);
		    Reg     (form);

		    R(form_list) = mcStart(Tuple_Value(format));
		    while (!fetch || mcNext(&R(item), &R(seq), ordered)) {
			fetch = false;
			if (mcNext(&R(form), &R(form_list), true)) {
			    if (Kind(R(form)) == String) {
				mcfprintf(f, "%s", Str_Value(R(form)), 0);
			    } else {
				mcPrintF(R(item), R(form), f);
				printed = true;
				fetch = true;
			    }
			} else {
			    if (!printed) {
				BAD_FORMAT(format, x);
			    }

			    /*
			     * restart format list, handle current
			     * item
			     */
			    R(form_list) = mcStart(Tuple_Value(format));
			}
		    }
		} break;

	      case Undefined:
	      case Integer:
	      case Real:
		while (mcNext(&R(item), &R(seq), ordered)) {
		    mcPrintF(R(item), format, f);
		}
		break;

	      default:
		BAD_FORMAT(format, x);
	    }
	} break;

      default:
	assert(0);
    }
    LEAVE;
}


#ifdef Screen
char    io_temp[Abs_Max_Out_Line+10];

void    mcEputs()
{
    char   *s = io_temp;

    while (*s)
	mcputc(*(s++));
}

void    mcEfputs(f)
    C_File  f;
{
    if (f == stdout)
	mcEputs();
    else
	fprintf(f, io_temp);
}

#endif


static void mcPrintOrdered(x,f,verbose) IPtr x; C_File f; Bool verbose; {
    ENTER;
    Reg(temp);
    R(temp) = PrintCopy(x);
    mcPrint(R(temp),f,verbose);
    LEAVE;
}
