#include "common.h"
#ifdef Mac
#ifdef GRAPHICS

#include "predef.h"
#include "screen.h"
#include "manager.h"
#include "micro.h"
#include "graph.h"

#include <ctype.h>


void pdXGraphics(x) Bool x; {
    if( x ) {
	if(Graphics) return;
        if( Val_Min_X == Val_Max_X ) {
	    Val_Min_X = -1;
	    Val_Max_X =  1;
	    Val_Min_Y = -1;
	    Val_Max_Y =  1;
        }

        opengraph();
        set_scale();
    
    /* Get MAXX MAXY XtoYaspect */
        MAXX=getmaxx(); MAXY=getmaxy();
        XtoYaspect = 1;

	InitialMax = MAXX;
	char_mult  = 1;

    clearviewport();
    
    } else {
	if(!Graphics) return;
	closegraph(); Graphics = false;
    }
}

void OPEN_GRAPH (void){pdXGraphics(true);}
void CLEAR_GRAPH(void){clearviewport();}
void CLOSE_GRAPH(void){pdXGraphics(false);}
void PENSIZE(short width){ linewidth = width; PenSize(width,width);}

Pd0(pdGetCoord) {
    ENTER;
    Reg(ans);
    Reg(Xval);
    Reg(Yval);
    Reg(Cval);
    char cstring[2];
    int x,y,c;
    char text[50];
 
    if( !Graphics ) {
        RT_ERROR("get_coord: No graphics window");
    }
    /* Enable Mouse tracking */
        StartMouseWatch();
    /* Receive Mouse position on mouse down or char typed */
    GraphicsUpdate(); 
    while( Graphics && DataC == -1 ) {
        quick_check();
    }
    /* Disable Mouse tracking */  
        EndMouseWatch();
    if( !Graphics ) {
        RT_ERROR("get_coord: No graphics window");
    }
    R(Xval) = New_Real( descaleX(DataX) );
    R(Yval) = New_Real( descaleY(DataY) );
    cstring[0] = DataC;
    cstring[1] = 0;
    R(Cval) = New_String( cstring );
    R(ans)  = mcCons(R(Cval), Nil);
    R(ans)  = mcCons(R(Yval), R(ans));
    R(ans)  = mcCons(R(Xval), R(ans));
    R(ans)  = New_Tuple(R(ans), def_orig);
    DataC = -1;

    LEAVE R(ans);
}



Pd0(pdRes) {
    ENTER;
    Reg(ans);

    R(ans) = Nil;
    R(ans) = mcCons(OM, R(ans));
    Info(R(ans)) = New_Real((Val_Max_Y-Val_Min_Y)/MAXY);
    R(ans) = mcCons(OM, R(ans));
    Info(R(ans)) = New_Real((Val_Max_X-Val_Min_X)/MAXX);

    R(ans) = New_Tuple(R(ans),1);

    LEAVE R(ans);
}

Pd0(pdXtoYratio) {
    return New_Real(1);
}
#endif
#endif
