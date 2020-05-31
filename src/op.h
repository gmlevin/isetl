/* $Header: op.h,v 3.1 90/04/25 16:25:04 gary Locked $ */
/*
 * Defines for Standard Operation Protocol Ops remain active until LEAVE.
 * ans is active.
 *
 * Ops and ans are registers.
 */


#define UNARY_ENTER					\
	IPtr *Only_Op  = Top;				\
	IPtr *Old_Top  = Top;				\
	Reg(ans)

#define NO_UNARY_ENTER					\
	IPtr *Only_Op  = Top;				\
	IPtr *Old_Top  = Top

#define UNARY_LEAVE					\
	Top = Old_Top; R(Only_Op) = R(ans); return

#define UNARY_NO_RESULT					\
	Top = Old_Top; mcPop(); return

#define BIN_ENTER					\
	IPtr *Left_Op  = Top-1,				\
	     *Right_Op = Top;				\
	IPtr *Old_Top  = Top;				\
	Reg(ans)

#define NO_BIN_ENTER					\
	IPtr *Left_Op  = Top-1,				\
	     *Right_Op = Top;				\
	IPtr *Old_Top  = Top

#define BIN_LEAVE					\
	Top = Old_Top; mcPop(); R(Left_Op) = R(ans); return

#define BIN_NO_RESULT					\
	Top = Old_Top; mcPop(); mcPop(); return

#define TRI_ENTER					\
	IPtr *First_Op  = Top-2,			\
	     *Second_Op = Top-1,			\
	     *Third_Op  = Top;				\
	IPtr *Old_Top   = Top;				\
	Reg(ans)

#define TRI_LEAVE					\
	Top = Old_Top; mcPop(); mcPop(); R(First_Op) = R(ans); return

#define GO_TO( lab )	EP_Ptr(PC) = lab; EP_Offset(PC) = 0;
