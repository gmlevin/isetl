/* $Header: mc_stack.c,v 3.1 90/04/25 16:23:20 gary Locked $ */
#include "common.h"
#include "micro.h"
#include "manager.h"



#ifndef mcTopPop
/* () Pop stack and return old top value */
IPtr     mcTopPop()
{
    IPtr     x;

    if (Top <= AStack)
	RT_ERROR("Stack Underflow");
    x = R(Top);
    Top--;
    return x;
}
#endif


/* () Is stack empty? */
Bool    mcIsEmpty()
{
    return Top <= AStack;
}

/* () Make stack empty */
void    mcMkEmpty()
{
    Top = AStack;
}



/* Push label L on stack */
void    mcPLabel(L)
    C_Label L;
{
    Reg     (new);

    R(new) = New_Block(Label, 0);
    Label_Value(R(new)) = L;
}
