#include "common.h"
/* $Header: emitter.c,v 3.1 90/04/25 16:22:26 gary Locked $ */
#include "parser.h"
#include "emitter.h"
#include "instr.h"
#include "manager.h"
#include "op.h"
#include "micro.h"
#include "ast.h"




/*
 * Store instruction(data) at PC and advance PC. Optionally print
 * instruction(data) on stdout.
 *
 * Includes Emit, Out_X (X=Integer,Ref,Env,Real,String,SRC,Label)
 */
void    Emit(instr)
    Instruction instr;
{
    if (instruction_printing)
	mcprintf("%d\t%s\n",
	       EP_Offset(PC), Inst_Map[(int) instr].Name);

    al_Instr;
    if (EP_Offset(PC) + Inst_Mult >= Max_Code)
	Flush_Code();
    *(C_Instr *) ((char *) CData(EP_Ptr(PC)) + EP_Offset(PC))
	= (C_Instr) instr;
    EP_Offset(PC) += Inst_Mult;
}

void    Out_Integer(s)
    char   *s;
{
    ENTER;
    Reg     (p);

    if (instruction_printing)
	mcprintf("int = %s\n", s,0);
    R(p) = mpStrToInt(s);
    Out_Const(R(p));
    LEAVE;
}


void    Out_Ref(n)
    C_Ref   n;
{
    if (instruction_printing)
	mcprintf("ref = %d\n", n,0);

    al_Instr;
    *(C_Instr *) ((char *) CData(EP_Ptr(PC)) + EP_Offset(PC))
	= (C_Instr) PRef;
    EP_Offset(PC) += Inst_Mult;

    al_Ref;
    *(C_Ref *) ((char *) CData(EP_Ptr(PC)) + EP_Offset(PC)) = n;
    EP_Offset(PC) += Ref_Mult;
    if (EP_Offset(PC) >= Max_Code)
	Flush_Code();
}

void    Out_Env(n)
    C_Ref   n;
{
    if (instruction_printing)
	mcprintf("ref = %d\n", n,0);

    al_Instr;
    *(C_Instr *) ((char *) CData(EP_Ptr(PC)) + EP_Offset(PC))
	= (C_Instr) Env_Extend;
    EP_Offset(PC) += Inst_Mult;

    al_Ref;
    *(C_Ref *) ((char *) CData(EP_Ptr(PC)) + EP_Offset(PC)) = n;
    EP_Offset(PC) += Ref_Mult;
    if (EP_Offset(PC) >= Max_Code)
	Flush_Code();
}

void    Out_Real(n)
    C_Real  n;
{
    ENTER;
    Reg     (p);

    if (instruction_printing)
	mcprintf("real = %g\n", n,0);
    R(p) = New_Real(n);
    Out_Const(R(p));

    LEAVE;
}

void    Out_String(s)
    char   *s;
{
    ENTER;
    Reg     (p);

    if (instruction_printing)
	mcprintf("string = \"%s\"\n", s,0);
    R(p) = New_String(s);
    Out_Const(R(p));

    LEAVE;
}
void    Out_SRC(n)
    IPtr     n;
{
    C_Label at;

    Flush_Code();
    CNext(Tail_Code) = New_Block(SRC, 0);
    at = CNext(Tail_Code);
    if (instruction_printing)
	mcprintf("Source code:\t",0,0);
    Tail_Code = at;
    CPtr(at) = n;
    CNext(at) = New_Block(Join, 0);
    Tail_Code = CNext(Tail_Code);

    if (instruction_printing)
	mcprintf("Join %lx\n", Tail_Code,0);
}

/* (n) Stores n.  */
void Out_Const(n)
    IPtr     n;
{
    C_Label at;

    Flush_Code();
    CNext(Tail_Code) = New_Block(PUSH, 0);
    at = CNext(Tail_Code);
    if (instruction_printing)
	mcprintf("Push Const\t",0,0);
    Tail_Code = at;
    CPtr(at) = n;
    CNext(at) = New_Block(Join, 0);
    Tail_Code = CNext(Tail_Code);

    if (instruction_printing)
	mcprintf("Join %lx\n", Tail_Code,0);
}


/* (n) Stores n.  returns where it was stored */
C_Label Out_Label(n)
    C_Label n;
{
    C_Label at;

    Flush_Code();
    CNext(Tail_Code) = New_Block(Fork, 0);
    at = CNext(Tail_Code);
    if (instruction_printing)
	mcprintf("at %lx Push %lx\t", at, n);
    Tail_Code = at;
    CPtr(at) = n;
    CNext(at) = New_Block(Join, 0);
    Tail_Code = CNext(Tail_Code);

    if (instruction_printing)
	mcprintf("Join %lx\n", Tail_Code,0);
    return at;
}



/* (a,b) Emits b at a */
void    Fix_Label(a, b)
    C_Label a,
            b;
{
    if (instruction_printing)
	mcprintf("Fixup at %lx to be %lx\n", a, b);
    CPtr(a) = b;
}



/* Initialize Code Queue */
void    Start_Code()
{
    GO_TO(&Working_Code);
    Kind(&Working_Code) = Code;
    CNext(&Working_Code) = Nil;
    New_Code = Tail_Code = New_Block(Join, 0);

    if (instruction_printing)
	mcprintf("Start code at %lx\n", New_Code,0);
}

/* Finish Code Queue */
void    Stop_Code()
{
    Flush_Code();
}

/*
 * Copy code from Working_Code to a Code_Block. Need enough room for the
 * code + a Cont instruction.
 */
void    Flush_Code()
{
    int     i;

    assert(EP_Offset(PC) <= Max_Code + 2 * Gen_Mult);
    if (EP_Offset(PC) == 0)
	return;

    /* Create a new code block */
    al_Instr;
    CNext(Tail_Code) = New_Block(Code, EP_Offset(PC) + sizeof(C_Instr));
    Tail_Code = CNext(Tail_Code);
    if (instruction_printing)
	mcprintf("Flush %lx\t", Tail_Code,0);

    /* Copy code */
    for (i = 0; i < EP_Offset(PC); i++) {
	CData(Tail_Code)[i] = CData(EP_Ptr(PC))[i];
    }

    /* emit Continue (Can't use Emit, because Block could be full */
    *(C_Instr *) ((char *) CData(Tail_Code) + EP_Offset(PC))
	= (C_Instr) Cont;

    /* Create a new Join node */
    CNext(Tail_Code) = New_Block(Join, 0);
    Tail_Code = CNext(Tail_Code);
    if (instruction_printing)
	mcprintf("Join %lx\n", Tail_Code,0);

    /* Reset PC */
    GO_TO(&Working_Code);
}



void    Emit_Op(op) int op;
{
    switch (op) {
      case T_And:
	Emit(And);
	break;
      case T_Div:
	Emit(Div);
	break;
      case T_Iff:		       /* Like Eq but with different
				        * precedence */
      case T_Eq:
	Emit(Eq);
	break;
      case T_Exp:
	Emit(Exp);
	break;
      case T_Ge:
	Emit(Ge);
	break;
      case T_Gt:
	Emit(Gt);
	break;
      case T_Impl:
	Emit(Impl);
	break;
      case T_In:
	Emit(In);
	break;
      case T_Le:
	Emit(Le);
	break;
      case T_Less:
	Emit(Less);
	break;
      case T_Lt:
	Emit(Lt);
	break;
      case T_Minus:
	Emit(Minus);
	break;
      case T_Mod:
	Emit(Mod);
	break;
      case T_Ne:
	Emit(Ne);
	break;
      case T_Notin:
	Emit(Notin);
	break;
      case T_Or:
	Emit(Or);
	break;
      case T_Plus:
	Emit(Plus);
	break;
      case T_Slash:
	Emit(Slash);
	break;
      case T_Subset:
	Emit(Subset);
	break;
      case T_Times:
	Emit(Times);
	break;
      case T_With:
	Emit(With);
	break;
      case '@':
	Emit(Orig);
	break;
      default:
	assert(0);
    }
}
