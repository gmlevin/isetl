/* $Header: emitter.h,v 3.1 90/04/25 16:24:47 gary Locked $ */
/*
 * Code is stored in four kinds of blocks:
 *	Code	-- straight line code without pointers.
 *	Join	-- no ops.  targets for branches.
 *	Fork	-- push a label.
 *	PUSH    -- push a constant.
 *
 * A sequence of code is linked through the CNext field.
 * A sequence is always of the form: Join ( (Code|Fork|PUSH) Join )*
 */



/*
 * Round up EP_Offset(PC) to smallest multiple that is >= EP_Offset(PC)
 */

#define al_Instr	( EP_Offset(PC) = (EP_Offset(PC) + (Inst_Mult-1))\
					& ~(Inst_Mult-1) )
#define al_Ref		( EP_Offset(PC) = (EP_Offset(PC) + (Ref_Mult-1))\
					& ~(Ref_Mult-1) )

#define Get_Instr	\
    ( al_Instr, EP_Offset(PC) += Inst_Mult, \
	*(C_Instr*)((char*)CData(EP_Ptr(PC))+EP_Offset(PC)-Inst_Mult) )
#define Get_Ref \
    ( al_Ref, EP_Offset(PC) += Ref_Mult, \
	*(C_Ref*)((char*)CData(EP_Ptr(PC))+EP_Offset(PC)-Ref_Mult) )





/*
 * Store instruction(data) at PC and advance PC. Optionally print
 * instruction(data) on stdout.
 */
extern void Flush_Code(NONE), Emit(C_INSTR),
        Out_Const(PTR),
        Out_Integer(STR),
        Out_Real(C_REAL),
        Out_String(STR), Out_Ref(C_REF),
	Out_Env(C_REF), Out_SRC(PTR);

extern C_Label Out_Label(C_LABEL);     /* (n) Stores n.  returns where it
				        * was stored */
extern void Fix_Label(C_LABEL X C_LABEL);	/* (a,b) Emits b at a */
extern void Start_Code(NONE);	       /* Initialize Code Queue */
extern void Stop_Code(NONE);	       /* Finish Code Queue */

extern Exec_Point PC;


extern struct Block Working_Code;
extern IPtr New_Code,
        Tail_Code;		       /* Code Queue */

/* New_Code --> Join --> Code .... -> Join  <-- Tail_Code */
