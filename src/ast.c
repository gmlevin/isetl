#include "common.h"
/* $Header: ast.c,v 3.2 90/04/25 16:22:13 gary Locked $ */
#include "ast.h"
#include "parser.h"
#include "instr.h"
#include "emitter.h"
#include "manager.h"
#include "micro.h"
#include "op.h"
#include "symbol.h"

static C_Ref bound_list[Max_Vars];
static int bounded = 0;
enum func_level{prog,func,proc};
static enum func_level nesting;

AST_Ptr Live_AST;

/* forward declarations for statics */
AST_Ptr Copy_AST(AST_PTR);
void CheckBound(AST_PTR);
void AST_Reset() {
   bounded = 0;
   Live_AST = Nil;
   nesting = prog;
}


/* Return an AST node */
AST_Ptr New_AST(k, f1, f2)
    int     k;
    AST_Ptr f1,
            f2;
{
    AST_Ptr new;

    new = New_Block(AST, 0);
    if (new == NULL)
	FATAL("Parser out of memory");
    ast_kind(new) = k;
    ast_1(new) = f1;
    ast_2(new) = f2;
    return new;
}

void w_DEREF(PTR);

/* @expr | @(expr, expr.list) */
/* <DEREF, expr, expr.list(reversed)> */
static void w_DEREF(T) IPtr T; {
    w_Expr(ast_1(T));
    w_Expr_List(ast_2(T),true);
    Emit(Cons);
    Emit(LookUp);
 }

/* Generate code for an Expression */
void    w_Expr(T)
    AST_Ptr T;
{
    switch (ast_kind(T)) {
      case DEREF:
        w_DEREF(T);
        Emit(Load);
        break;
      case T_If:
	w_If(T, true);
	break;

      case T_Missing:
	SYNTAX("~ not permitted");
	ERROR;
	break;

      case T_Where:
	w_Where(T);
	break;

      case T_Func:
      case T_Proc:
	w_Func(T);
	break;

      case T_False:
	Emit(PFalse);
	break;

      case T_Integer:
	Out_Integer((char *) (ast_1(T)));
	break;

      case T_Newat:
	Emit(PAtom);
	break;

      case T_Om:
	Emit(POM);
	break;

      case T_Real:{
	    C_Real  n;

#ifdef Double
	    char   *form = "%lf";

#else
	    char   *form = "%f";

#endif

	    sscanf((char *) (ast_1(T)), form, &n);
	    Out_Real(n);
	} break;

      case T_String:
	Out_String((char *) (ast_1(T)));
	break;

      case T_True:
	Emit(PTrue);
	break;

      case T_Not:
	w_Expr(ast_2(T));
	Emit(Not);
	break;

      case T_With:
      case T_Less:
      case T_Iff:
      case T_Div:
      case T_Eq:
      case T_Exp:
      case T_Ge:
      case T_Gt:
      case T_Impl:
      case T_In:
      case T_Le:
      case T_Lt:
      case T_Mod:
      case T_Ne:
      case T_Notin:
      case T_Slash:
      case T_Subset:
      case T_Times:
      case '@':
	w_Expr(ast_1(T));
	w_Expr(ast_2(T));
	Emit_Op(ast_kind(T));
	break;

      case T_Plus:
	if (ast_1(T) == ANULL) {       /* Unary Plus */
	    w_Expr(ast_2(T));
	} else {		       /* Binary Plus */
	    w_Expr(ast_1(T));
	    w_Expr(ast_2(T));
	    Emit(Plus);
	} break;

      case T_Minus:
	if (ast_1(T) == ANULL) {       /* Unary Minus */
	    w_Expr(ast_2(T));
	    Emit(Negate);
	} else {		       /* Binary Minus */
	    w_Expr(ast_1(T));
	    w_Expr(ast_2(T));
	    Emit(Minus);
	} break;

      case T_And:		       /* Implemented as conditional and */
	{
	    C_Label test,
	            skip;

	    w_Expr(ast_1(T));
	    test = Out_Label(LNULL);
	    Emit(Br_False);

	    w_Expr(ast_2(T));
	    skip = Out_Label(LNULL);
	    Emit(Br);

	    Fix_Label(test, Make_Join());
	    Emit(PFalse);
	    Fix_Label(skip, Make_Join());
	} break;

      case T_Or:		       /* Implemented as conditional or */
	{
	    C_Label test,
	            skip;

	    w_Expr(ast_1(T));
	    test = Out_Label(LNULL);
	    Emit(Br_True);

	    w_Expr(ast_2(T));
	    skip = Out_Label(LNULL);
	    Emit(Br);

	    Fix_Label(test, Make_Join());
	    Emit(PTrue);
	    Fix_Label(skip, Make_Join());
	} break;

      case '?':		       /* conditional OM */
	{
	    C_Label done;

	    w_Expr(ast_1(T));
	    Emit(Dup);
	    Emit(POM);
	    Emit(Eq);
	    done = Out_Label(LNULL);
	    Emit(Br_False);
	    Emit(Pop);
	    w_Expr(ast_2(T));
	    Fix_Label(done, Make_Join());
	} break;

      case '#':		       /* count */
	w_Expr(ast_1(T));
	Emit(Count);
	break;

      case CALL:		       /* <CALL, expr, NULL> */
	w_Expr(ast_1(T));
	Emit(Pop);
	break;

      case T_Id:		       /* id */
	Out_Ref(find_id((char *) (ast_1(T))));
	Emit(Load);
	break;

      case T_Insert:
	w_Insert(T);
	break;

      case T_Exists:
      case T_Choose:
      case T_Forall:
	w_Quantifier(T);
	break;

      case SELECTOR:		       /* < SELECTOR expr selector > */
	w_Expr(ast_1(T));
	w_Sel(ast_2(T));
	break;

      case Set_Open:		       /* set former */
	w_Former(ast_1(T));
	Emit(Mk_Set);
	break;

      case Tuple_Open:		       /* tuple former */
	w_Former(ast_1(T));
	Emit(Mk_Tuple);
	break;

      default:
	assert(0);
    }
}

/* Former: empty | expr T_St iterator | expr.list | counted */
void    w_Former(T)
    AST_Ptr T;
{
    if (T == ANULL) {		       /* Empty */
	Emit(PNil);

    } else if (ast_kind(T) == T_St) {  /* Generator */
	/* AST_1 = expr AST_2 = <ITERS, iter.list, cond> */
	AST_Ptr iter_list = ast_2(T);
	C_Label next_label,
	        exit_br;
	C_Ref   anon = Mk_Var();
	C_Ref   old_bound = bounded;

	/* Create empty anonymous variable */
	    Out_Ref(anon);
	    Emit(PNil);
	    Emit(Assign);

	Emit(PNil);
	w_Iter(ast_1(iter_list), &next_label, &exit_br);

	/* Condition code */
	if (ast_2(iter_list) != ANULL) {
	    w_Expr(ast_2(iter_list));
	    Out_Label(next_label);
	    Emit(Br_False);
	}
	/* anon := Cons( expr, anon ) */
	    Out_Ref(anon);
		w_Expr(ast_1(T));
		Out_Ref(anon);
		Emit(Load);
		Emit(Cons);
	    Emit(Assign);

	/* Iterate */
	Out_Label(next_label);
	Emit(Br);

	Fix_Label(exit_br, Make_Join());
	Emit(Cl_Iter);

	/* Leave anon on stack */
	    Out_Ref(anon);
	    Emit(Load);
	    Emit(Rev);

        /* Clear anon */
	    Out_Ref(anon);
	    Emit(POM);
	    Emit(Assign);

	Free_Var();
	bounded = old_bound;

    } else if (ast_kind(T) == T_Dotdot) {	/* counted */
	AST_Ptr start = ast_1(T),
	        first = ast_1(start),
	        rest = ast_2(start),
	        stop = ast_2(T);

	if (rest == ANULL) {
	    w_Expr(first);
	    w_Expr(stop);
	    Emit(Cnt2);
	} else if (ast_2(rest) == ANULL) {
	    w_Expr(ast_1(rest));       /* list is reversed */
	    w_Expr(first);
	    w_Expr(stop);
	    Emit(Cnt3);
	} else {
	    SYNTAX("Too many expressions before ..");
	    ERROR;
	}

    } else {			       /* Expr list */
	w_Expr_List(T, true);
    }
}

#define AddBound 1
#define UnBound	 2
#define PushRef	 3
#define APushRef 4

/* <T_Gets, LHS, EXPR>	 */
void    w_Assign(T)
    AST_Ptr T;
{
    switch (ast_kind(T)) {
      case T_Gets:
	w_LHS(ast_1(T), PushRef);
	w_Expr(ast_2(T));
	Emit(Assign);
	break;

      default:
	assert(0);
    }
}


/*
 * <T_Id, str, NULL> | <SELECTOR, lhs, selector> | <'[', lhs.list, NULL>
 *
 * if bind, add to the Environment
 * if not bind, push on stack
 */
void    w_LHS(T, bind)
    AST_Ptr T;
    int     bind;
{
    switch (ast_kind(T)) {
      case DEREF:
	switch (bind) {
	  case AddBound:
            SYNTAX("Cannot bind `@(expr)'");
            ERROR;
	    break;

	  case PushRef:
            w_DEREF(T);
	    break;

	  default:
	    assert(0);
	}
        break;
      case T_Id: {
        C_Ref i = find_id((char *) (ast_1(T)));
        if( Locked(i) ) {
            SYNTAX("Cannot assign to locked id.");
	    mcprintf("ID = %s\n", (char*)ast_1(T),0);
            ERROR;
        }
	switch (bind) {
	  case AddBound: Out_Env(i); Bound(i); break;
	  case PushRef:  Out_Ref(i);           break;
	  case UnBound:  Emit(Env_Pop);        break;
	  default:       assert(0);
	}
	} break;

      case SELECTOR:
	switch (bind) {
	  case UnBound:
	  case AddBound:
	    SYNTAX("Only identifiers may be bounded");
	    ERROR;
	    break;

	  case PushRef:
	    w_LHS(ast_1(T), bind);
	    w_Sel(ast_2(T));
	    break;

	  default:
	    assert(0);
	}
	break;

      case '[':
	w_LHS_List(ast_1(T), true, bind);
	break;

      case T_Missing:
	switch (bind) {
	  case UnBound:
	  case AddBound:
	    /* nothing */
	    break;

	  case PushRef:
	    Emit(POM);
	    break;

	  default:
	    assert(0);
	}
	break;

      default:
	SYNTAX("Only identifiers can be bound");
	ERROR;
    }
}


/*
 * If any_list, generate a tuple for LHS.list list. O.W. generate list for
 * non-singletons only. APushRef is used to walk even elements in list as
 * LHS (odd as expr) (for ReadF)
 */
void    w_LHS_List(list, any_list, bind)
    AST_Ptr list;
    Bool    any_list;
    int     bind;
{
    Bool    alt = (bind == APushRef);

    if (alt)
	bind = PushRef;

    if (any_list || ast_2(list) != ANULL) {
	if (bind == PushRef)
	    Emit(PNil);
	while (list != ANULL) {
	    if (alt) {
		w_Expr(ast_1(list));
		list = ast_2(list);
		Emit(Swap);
		Emit(Cons);
	    }
	    w_LHS(ast_1(list), bind);
	    if (bind == PushRef) {
		Emit(Swap);
		Emit(Cons);
	    }
	    list = ast_2(list);
	}
	if (bind == PushRef)
	    Emit(Mk_Tuple);
    } else {			       /* singleton to be generated as a
				        * simple LHS */
	w_LHS(ast_1(list), bind);
    }
}

/*
 * Generate code for iterator list. Assumes that a previous iter is on the
 * stack.
 *	next_label = address to branch to to get next iteration
 *	 exit_br   = address to fix up for exit when iteration is done
 */
void    w_Iter(L, next_label, exit_br)
    AST_Ptr L;
    C_Label *next_label,
           *exit_br;
{
    AST_Ptr iter = ast_1(L);
    AST_Ptr rest = ast_2(L);

    /* Create first iterator */
    switch (ast_kind(iter)) {
      case T_In:{		       /* <T_In, lhs, expr> */
	    AST_Ptr lhs;

	    lhs = ast_1(iter);

	    w_LHS(lhs, AddBound);
	    w_LHS(lhs, PushRef);

	    if (ast_kind(ast_2(iter)) == Tuple_Open
		&& ast_kind(ast_1(ast_2(iter))) == T_Dotdot) {
		/* x in [counted] */
		AST_Ptr counter = ast_1(ast_2(iter));
		AST_Ptr start = ast_1(counter),
		        first = ast_1(start),
		        rest = ast_2(start),
		        stop = ast_2(counter);

		if (rest == ANULL) {
		    w_Expr(first);
		    w_Expr(stop);
		    Emit(Mk_2Cnt);
		} else if (ast_2(rest) == ANULL) {
		    w_Expr(ast_1(rest));	/* list is reversed */
		    w_Expr(first);
		    w_Expr(stop);
		    Emit(Mk_3Cnt);
		} else {
		    SYNTAX("Too many expressions before ..");
		    ERROR;
		}
	    } else {		       /* x in anything else */
		w_Expr(ast_2(iter));
		Emit(Mk_Iter);
	    }
	} break;

      case MAP:		       /* <MAP, e1, e2> */
	/* e1 should be a lhs */
	/* e2 should be <SELECTOR, id, selector> */
	/*     <Set_Open, lhs.list, NULL>	MMap */
	/*     <'(',	   lhs.list, NULL>	SMap */
	/*     <CALL,	   -,	     ->	    NOT ALLOWED */

	if (ast_kind(ast_2(iter)) != SELECTOR
	    || ast_kind(ast_1(ast_2(iter))) != T_Id
	    || ast_kind(ast_2(ast_2(iter))) == CALL
	    ) {
	    SYNTAX("Bad RHS for Iterator");
	    ERROR;
	}
	w_LHS_List(ast_1(ast_2(ast_2(iter))),
		   false, AddBound);
	w_LHS_List(ast_1(ast_2(ast_2(iter))),
		   false, PushRef);
	w_LHS(ast_1(iter), AddBound);
	w_LHS(ast_1(iter), PushRef);
	Emit(PNil);
	Emit(Cons);
	Emit(Cons);
	Emit(Mk_Tuple);
	w_Expr(ast_1(ast_2(iter)));
	switch (ast_kind(ast_2(ast_2(iter)))) {
	  case '(':
	    Emit(Mk_FIter);
	    break;

	  case Set_Open:
	    Emit(Mk_MIter);
	    break;

	  default:
	    SYNTAX("Bad Selector for Iterator");
	    ERROR;
	}
	break;

      default:
	assert(0);
    }

    /* Establish branch points */
    *next_label = Make_Join();
    *exit_br = Out_Label(LNULL);
    Emit(Iter_Next);

    if (rest != ANULL) {
	C_Label n2,
	        e2;

	w_Iter(rest, &n2, &e2);
	Fix_Label(e2, *next_label);
	*next_label = n2;
    }
}

/*
 * Emit code for Quantified expression T
 * <T_Exists|T_Forall, iter.list, cond>
 */
void    w_Quantifier(T)
    AST_Ptr T;
{
    C_Label next_label,
            exit_br,
            end_br;
    C_Ref   old_bound = bounded;

    Emit(PNil);
    w_Iter(ast_1(T), &next_label, &exit_br);
    w_Expr(ast_2(T));

    /* Repeat if witness not found */
    Out_Label(next_label);
    if (ast_kind(T) == T_Exists
	|| ast_kind(T) == T_Choose)
	Emit(Br_False);
    else
	Emit(Br_True);

    /* Witness found, dispose of iterator */
    if (ast_kind(T) == T_Choose) {
	/* Leave iterator value under iterator */
	/* lhs    first iter  */
	Emit(Dup);
	Emit(Iter_Load);
	Emit(Swap);
    }
    Emit(Cl_Iter);
    if (ast_kind(T) == T_Exists)
	Emit(PTrue);
    else if (ast_kind(T) == T_Choose) {
	/* iterator value already on stack */
    } else
	Emit(PFalse);

    end_br = Out_Label(LNULL);
    Emit(Br);

    /* No witness found */
    Fix_Label(exit_br, Make_Join());
    Emit(Cl_Iter);
    if (ast_kind(T) == T_Exists)
	Emit(PFalse);
    else if (ast_kind(T) == T_Choose)
	Emit(POM);
    else
	Emit(PTrue);

    Fix_Label(end_br, Make_Join());
    bounded = old_bound;
}

void    w_Stmt_List(T, need_expr)
    AST_Ptr T;
    Bool    need_expr;
{
    if (need_expr && ast_2(T) != ANULL) {
	SYNTAX("Statement list found where expression expected.");
	ERROR;
    }
    while (T != ANULL) {
	w_Stmt(ast_1(T), need_expr);
	T = ast_2(T);
    }
}


void    w_Stmt(T, need_expr)
    AST_Ptr T;
    Bool    need_expr;
{
    /* generate debug code from source nodes */
    while (T != ANULL && ast_kind(T) == T_Source) {
	IPtr     x = ast_1(T);

	if (x != ANULL)
	    Out_SRC(x);
	T = ast_2(T);
    }
    if (T == ANULL)
	return;

    if (need_expr && ast_kind(T) != CALL) {
	SYNTAX("Statement found where expression expected.");
	ERROR;
    }
    switch (ast_kind(T)) {
      case CALL:
	if (ast_kind(ast_1(T)) == T_If) {
	    w_If(ast_1(T), need_expr);
	} else {
	    w_Expr(ast_1(T));
	    if (!need_expr)
		Emit(Pop);
	}
	break;

      case T_Gets:
	w_Assign(T);
	break;

      case T_While:
	w_While(T);
	break;

      case T_Read:
      case T_ReadF:
	w_LHS_List(ast_1(T), true,
		   ast_kind(T) == T_Read ? PushRef : APushRef);
	if (ast_2(T) == ANULL) {
	    Emit(PNil);
	} else {
	    w_Expr(ast_2(T));
	}
	if (ast_kind(T) == T_Read)
	    Emit(Read);
	else
	    Emit(ReadF);
	break;

      case T_PrintF:
      case T_Print:
	w_Expr_List(ast_1(T), true);
	if (ast_2(T) == ANULL) {
	    Emit(PNil);
	} else {
	    w_Expr(ast_2(T));
	}
	if (ast_kind(T) == T_Print)
	    Emit(Print);
	else
	    Emit(PrintF);
	break;

      case T_For:
	w_For(T);
	break;

      case T_Return:
        if( nesting==prog ) {
            SYNTAX("`return' not in func or proc");
            ERROR;
        }
	if (ast_1(T) == ANULL) {
            if( nesting==func )	    Emit(POM);
            else                    Emit(PNP);
	} else {
	    if( nesting==func )	    w_Expr(ast_1(T));
            else { SYNTAX("`return expr' not permitted in proc"); ERROR;}
	}
	Emit(Return);
	break;

      case T_Frome:
	w_LHS(ast_1(T), PushRef);
	w_LHS(ast_2(T), PushRef);
	Emit(Frome);
	break;

      case T_Fromb:
	w_LHS(ast_1(T), PushRef);
	w_LHS(ast_2(T), PushRef);
	Emit(Fromb);
	break;

      case T_From:
	w_LHS(ast_1(T), PushRef);
	w_LHS(ast_2(T), PushRef);
	Emit(From);
	break;

      default:
	assert(0);
    }
}

/*
 * <T_If, <NULL, cond, then>, <NULL, else.ifs, else> then, else are
 * stmt.lists else.ifs are list of <NULL, cond, stmt.list>
 */
void    w_If(T, need_expr)
    AST_Ptr T;
    Bool    need_expr;
{
    C_Label test,
            done,
            done_br;
    AST_Ptr else_ifs = ast_1(ast_2(T));

    w_Expr(ast_1(ast_1(T)));	       /* cond */
    test = Out_Label(LNULL);	       /* test */
    Emit(Br_False);

    w_Stmt_List(ast_2(ast_1(T)), need_expr);	/* then */
    done_br = Make_Join();
    done = Out_Label(LNULL);	       /* exit */
    Emit(Br);

    while (else_ifs != ANULL) {
	Fix_Label(test, Make_Join());

	w_Expr(ast_1(ast_1(else_ifs)));
	test = Out_Label(LNULL);
	Emit(Br_False);

	w_Stmt_List(ast_2(ast_1(else_ifs)), need_expr);
	Out_Label(done_br);
	Emit(Br);

	else_ifs = ast_2(else_ifs);
    }

    Fix_Label(test, Make_Join());

    if (ast_2(ast_2(T)) != ANULL) {
	w_Stmt_List(ast_2(ast_2(T)), need_expr);
    } else if (need_expr) {
        Emit(POM);
    }
    Fix_Label(done, Make_Join());
}

/* <T_While, cond, stmt.list> */
void    w_While(T)
    AST_Ptr T;
{
    C_Label start,
            test;

    start = Make_Join();

    w_Expr(ast_1(T));
    test = Out_Label(LNULL);
    Emit(Br_False);

    w_Stmt_List(ast_2(T), false);
    Out_Label(start);
    Emit(Br);

    Fix_Label(test, Make_Join());
}


/* <T_For, <NULL, iter.list, cond>, stmt.list> */
void    w_For(T)
    AST_Ptr T;
{
    C_Label next_label,
            exit_br;
    C_Ref   old_bound = bounded;

    Emit(PNil);
    w_Iter(ast_1(ast_1(T)), &next_label, &exit_br);

    if (ast_2(ast_1(T)) != ANULL) {
	w_Expr(ast_2(ast_1(T)));       /* cond */
	Out_Label(next_label);
	Emit(Br_False);		       /* skip body */
    }
    w_Stmt_List(ast_2(T), false);
    Out_Label(next_label);
    Emit(Br);			       /* repeat */

    Fix_Label(exit_br, Make_Join());
    Emit(Cl_Iter);
    bounded = old_bound;
}

/*
 * selectors = <Set_Open, exp.list, NULL>	MMap <'(',
 * exp.list, NULL>	SMap <CALL,	   NULL,     NULL>	SMap (No
 * parameters) <T_Dotdot, low, high>		Slice
 *
 * single element lists should be treated as singletons. >single element
 * lists should be converted into tuples. low or high may be null.  null
 * => Nil.
 */
void    w_Sel(select)
    AST_Ptr select;
{
    switch (ast_kind(select)) {
      case CALL:
      case '(':{
	    w_Expr_List(ast_1(select), false);
	    Emit(SMap);
	    Flush_Code();	       /* Provides label for return */
	} break;

      case T_Dotdot:{
	    if (ast_1(select) == ANULL)
		Emit(PNil);
	    else
		w_Expr(ast_1(select));
	    if (ast_2(select) == ANULL)
		Emit(PNil);
	    else
		w_Expr(ast_2(select));
	    Emit(Slice);
	} break;

      case Set_Open:{
	    w_Expr_List(ast_1(select), false);
	    Emit(MMap);
	} break;

      default:
	assert(0);
    }
}


/*
 * If any_list, generate a list for expr.list. O.W. generate List for
 * non-singletons, value for singletons
 *
 * Creates list in REVERSE order that was passed
 */
void    w_Expr_List(list, any_list)
    AST_Ptr list;
    Bool    any_list;
{
    if (any_list || list == ANULL
	|| ast_2(list) != ANULL) {
	Emit(PNil);
	while (list != ANULL) {
	    w_Expr(ast_1(list));
	    Emit(Swap);
	    Emit(Cons);
	    list = ast_2(list);
	}
    } else {			       /* singleton to be generated as a
				        * simple Expr */
	w_Expr(ast_1(list));
    }
}


/* create and return a join code block */
IPtr     Make_Join()
{
    Flush_Code();
    return Tail_Code;
}


/* include al_Ref when needed */
#define Gen_Ref(n) \
	((C_Ref *)CData(EP_Ptr(PC)))[EP_Offset(PC)/Ref_Mult] = n; \
	EP_Offset(PC) += Ref_Mult; \
	if( EP_Offset(PC) >= Max_Code ) FATAL("Too many locals")

/* <T_Func, < NULL, doc, args>, <NULL, dcls, body>>
   doc = list of strings
   args = <NULL,parms,opt> 
 */
/*
 * Leave a closure consisting of a pointer to the body of the function and
 * the current environment.
 *
 * Separately, create code for the body. Need to add any anonymous variables
 * in body to locals, before the Mk_Env code is generated for the body.
 *
 * dcls = list of id_lists, each node marked T_Local or T_Value
 */
void    w_Func(T)
    AST_Ptr T;
{
    AST_Ptr dcls,
            p;
    C_Label body_label;
    C_Ref   Old_Anons;
    enum func_level old_nesting = nesting;

    nesting = ast_kind(T)==T_Func ? func : proc;

    /* Make the closure for the code body */
    body_label = Out_Label(LNULL);
    /* add values to env */
    for (dcls = ast_1(ast_2(T));
	 dcls != ANULL;
	 dcls = ast_2(dcls)) {
	if (ast_kind(dcls) == T_Value)
	    for (p = ast_1(dcls);
		 p != ANULL;
		 p = ast_2(p)) {
		C_Ref   id = find_id((char *) (ast_1(ast_1(p))));

		/* Fetch current value */
		Out_Ref(id);
		Emit(Load);
		/* Create local copy */
		Out_Env(id);
		Out_Ref(id);
		/* Store value in local copy */
		Emit(Swap);
		Emit(Assign);
	    }
    }
    Out_Const(ast_1(ast_1(T)));  /* documentation */
    Emit(Mk_Closure);
    /* delete values from env */
    for (dcls = ast_1(ast_2(T));
	 dcls != ANULL;
	 dcls = ast_2(dcls)) {
	if (ast_kind(dcls) == T_Value)
	    for (p = ast_1(dcls);
		 p != ANULL;
		 p = ast_2(p)) {
		Emit(Env_Pop);
	    }
    }
    Flush_Code();		       /* Clears Working_Code resets PC */

    /* Save current info (New_Code, Tail_Code, Anons) */
    mcPush(New_Code);
    mcPush(Tail_Code);
    Old_Anons = Anons;

    Start_Code();
    Fix_Label(body_label, New_Code);

    /* Emit Mk_Env and Flush (Creating Join), will fix up later */
    Emit(Mk_Env);
    Flush_Code();

    /* Emit code for body */
    w_Stmt_List(ast_2(ast_2(T)), false);
    if( nesting==func )	    Emit(POM);
    else                    Emit(PNP);

    Emit(Return);
    Stop_Code();

    /* Fix Mk_Env (list of parms, list of locals, Anons..-1) */
    /* Working_Code is clear.  */
    /* Emit and then re-link at front of body_code */
    Emit(Mk_Env);
    al_Ref;  /* as long as only generate references not needed */
    {
	C_Ref   i;
	C_Label body_code,
	        old_tail;
	IPtr     args = ast_2(ast_1(T)),
	        parms= ast_1(args),
	        opts = ast_2(args);

	/* Create parms */
	for (p = parms; p != ANULL; p = ast_2(p)) {
	    Gen_Ref(find_id((char *) (ast_1(ast_1(p)))));
	}
	Gen_Ref((C_Ref) 0);

	/* Create opts */
	for (p = opts; p != ANULL; p = ast_2(p)) {
	    Gen_Ref(find_id((char *) (ast_1(ast_1(p)))));
	}
	Gen_Ref((C_Ref) 0);

	/* Create locals */
	for (dcls = ast_1(ast_2(T));
	     dcls != ANULL;
	     dcls = ast_2(dcls)) {
	    if (ast_kind(dcls) == T_Local)
		for (p = ast_1(dcls);
		     p != ANULL;
		     p = ast_2(p)) {
		    Gen_Ref(find_id((char *) (ast_1(ast_1(p)))));
		}
	}

	/* Create anons */
	for (i = Anons; i < 0; i++) {
	    Gen_Ref(i);
	}
	Gen_Ref((C_Ref) 0);


	/* Grab code for body (Skip Join--MkEnv) */
	body_code = CNext(CNext(New_Code));
	old_tail = Tail_Code;

	/* Temporarily attach new MkEnv to end */
	Flush_Code();

	/* Splice new MkEnv at beginning and disconnect it from end */
	CNext(New_Code) = CNext(old_tail);
	CNext(CNext(New_Code)) = body_code;
	Tail_Code = old_tail;
	CNext(Tail_Code) = Nil;
    }
    /* Restore current info */
    Anons = Old_Anons;
    Tail_Code = mcTopPop();
    New_Code = mcTopPop();
    GO_TO(&Working_Code);

    nesting = old_nesting;
}



/* Destructively  reverse rev */
AST_Ptr Rev_Seq(rev)
    AST_Ptr rev;
{
    AST_Ptr done,
            t;

    /* reverse rev into done */
    done = ANULL;
    while (rev != ANULL) {
	t = ast_2(rev);
	ast_2(rev) = done;
	done = rev;
	rev = t;
    }
    return done;
}



/*
 * <T_Insert, binop, <NULL, left, right> > <T_Insert, binop, <NULL, NULL,
 * right> > binop = id | <'(', expr,ANULL> | operator
 */
void    w_Insert(T)
    AST_Ptr T;
{
    AST_Ptr binop = ast_1(T);
    AST_Ptr left = ast_1(ast_2(T));
    AST_Ptr right = ast_2(ast_2(T));
    C_Ref   x = Mk_Var(),
            y = Mk_Var(),
            S = Mk_Var(),
            op = Mk_Var();
    C_Label start,
            test;

/*
 * Generate code as for S := right; x := left; ( take x from S, if no left
 * arg) for y in S do x := x op y end Leave x on stack
 */

    if (ast_kind(binop) == T_Id) {
	Out_Ref(op);
	w_Expr(binop);
	Emit(Assign);
    } else if (ast_kind(binop) == '(') {
	Out_Ref(op);
	w_Expr(ast_1(binop));
	Emit(Assign);
    }
    Out_Ref(S);
    w_Expr(right);
    Emit(Assign);

    if (left != ANULL) {
	Out_Ref(x);
	w_Expr(left);
	Emit(Assign);
    } else {
	Out_Ref(x);
	Out_Ref(S);
	Emit(From);
    }


    /* For */
    Out_Ref(x);
    Emit(Load);

    Emit(PNil);
    Out_Env(y);
    Out_Ref(y);
    Out_Ref(S);
    Emit(Load);
    Emit(Mk_Iter);

    start = Make_Join();
    test = Out_Label(LNULL);
    Emit(Iter_Next);

    /* Do (top == iter, second == x ) */
    /* x := x .op y */
    Emit(Swap);
    Out_Ref(y);
    Emit(Load);

    if (ast_kind(binop) == T_Id || ast_kind(binop) == '(') {
	Emit(PNil);
	Emit(Cons);
	Emit(Cons);
	Out_Ref(op);
	Emit(Load);
	Emit(Swap);
	Emit(SMap);
	Flush_Code();		       /* Provides label for return */
    } else {
	Emit_Op(ast_kind(binop));
    }
    Emit(Swap);


    /* End */
    Out_Label(start);
    Emit(Br);

    Fix_Label(test, Make_Join());
    Emit(Cl_Iter);

    Free_Var();			       /* x */
    Free_Var();			       /* y */
    Free_Var();			       /* S */
    Free_Var();			       /* op */
}



/*
 * <T_Where, expr0, defns> defns are a list of pairs "T_Gets": <bound,
 * expr> or  "SELECTOR": <lhs, expr>
 *
 * Evaluate "expr0" in an environment with "bound" bound to "expr".  "expr"
 * is evaluated in an environment where all the "bound"s are local, and
 * the bindings to values occur in the order written.
 *
 * "lhs" must be of form "id selector".
 */

void    w_Where(T)
    AST_Ptr T;
{
    AST_Ptr expr0 = ast_1(T);
    AST_Ptr defns = ast_2(T),
            d;
    C_Ref   old_bound = bounded;

    /* add all bound vars to the environment */
    for (d = defns; d != ANULL; d = ast_2(d)) {
	AST_Ptr lhs = ast_1(ast_1(d));

	if (ast_kind(ast_1(d)) == T_Gets) {
	    w_LHS(lhs, AddBound);
	} else {
	    CheckBound(lhs);
	}
    }

    /* Perform all the assignments */
    for (d = defns; d != ANULL; d = ast_2(d)) {
	AST_Ptr lhs = ast_1(ast_1(d));
	AST_Ptr expr = ast_2(ast_1(d));

	w_LHS(lhs, PushRef);
	w_Expr(expr);
	Emit(Assign);
    }

    /* evaluate the expression */
    w_Expr(expr0);

    /* clear the environment */
    for (d = defns; d != ANULL; d = ast_2(d)) {
	if (ast_kind(ast_1(d)) == T_Gets) {
	    AST_Ptr lhs = ast_1(ast_1(d));

	    w_LHS(lhs, UnBound);
	}
    }
    bounded = old_bound;
}



static AST_Ptr Copy_AST(T)
    AST_Ptr T;
{
    if (T == ANULL)
	return ANULL;
    switch (ast_kind(T)) {
      case T_Id:
      case T_Integer:
      case T_Real:
      case T_String:
	return New_AST(ast_kind(T), (AST_Ptr) str((char *) (ast_1(T))),
		       ANULL);
      default:
	return New_AST(ast_kind(T),
		       Copy_AST(ast_1(T)),
		       Copy_AST(ast_2(T)));
    }
}



/*
 * Spread(iter, list) Returns cons(iter, list). If "iter" is <T_In,
 * bound.list, expr>, append a COPY of <T_In,bound,expr> for each element
 * in bound.list. if "iter" is <MAP, bound, expr> no copying needed.
 */
AST_Ptr Spread(PTR X PTR);
AST_Ptr Spread(iter, list)
    AST_Ptr iter,
            list;
{
    switch (ast_kind(iter)) {
      case T_In:{
	    AST_Ptr p;
	    AST_Ptr expr = ast_2(iter);

	    for (p = ast_1(iter); p != ANULL; p = ast_2(p)) {
		list = New_AST(0, New_AST(T_In, Copy_AST(ast_1(p)),
					  Copy_AST(expr)),
			       list);
	    }
	    return list;
	} break;

      case MAP:{
	    return New_AST(0, iter, list);
	} break;

      default:
	SYNTAX("Bad Iterator");
	ERROR;
    }
    return ANULL;
}

/* Warn about multiple binding of ids */
void    Bound(id)
    C_Ref   id;
{
    int     i;

    for (i = 0; i < bounded; i++) {
	if (id == bound_list[i]) {
	    SYNTAX("warning -- id bound twice in same scope");
	    return;
	}
    }
    bound_list[bounded] = id;
    bounded++;
}

/* Error if selector not bound */
static void    CheckBound(lhs)
    AST_Ptr lhs;
{
    C_Ref   id;
    int     i;

    if (ast_kind(lhs) != SELECTOR)
	return;
    id = find_id( (char*)ast_1(ast_1(lhs)));
    for (i = 0; i < bounded; i++) {
	if (id == bound_list[i]) {
	    return;
	}
    }
    SYNTAX("Error -- id modified before bound in where");
    ERROR;
}

/* <T_Prog, dcls, stmt.list> */
void w_Prog(T) AST_Ptr T; {
    AST_Ptr dcls, d;
    for( dcls = ast_1(T); dcls !=  ANULL; dcls = ast_2(dcls) ) {
	if( ast_kind(dcls) == T_Local ) { /* Extend the environment */
	    for( d = ast_1(dcls)  ; d != ANULL; d = ast_2(d) ) {
		Emit(Env_Extend);
		al_Ref;
		Gen_Ref(find_id((char *) (ast_1(ast_1(d)))));
	    }
	} else { /* Extend environment with initial values */
	    for( d = ast_1(dcls)  ; d != ANULL; d = ast_2(d) ) {
		C_Ref id = find_id((char *) (ast_1(ast_1(d))));
		Out_Ref(id);
		Emit(Load);
		Emit(Env_Extend);
		al_Ref;
		Gen_Ref(id);
		Out_Ref(id);
		Emit(Swap);
		Emit(Assign);
	    }
	}
    }
    w_Stmt_List(ast_2(T), false);
    
    /* clear environment */
    for( dcls = ast_1(T); dcls !=  ANULL; dcls = ast_2(dcls) ) {
	for( d = ast_1(dcls)  ; d != ANULL; d = ast_2(d) ) {
	    Emit(Env_Pop);
	}
    }
}


/* Return an new copy of string p */
    char * str(p) char * p; {
	char * new = malloc(strlen(p) + 1);
	if( new == NULL ) {
	    FATAL("Out of parsing space.");
	}
	strcpy(new,p);
	return new;
    }

