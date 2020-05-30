#include "common.h"
#ifdef GRAPHICS

#include "predef.h"
#include "screen.h"
#include "manager.h"
#include "micro.h"
#include "graph.h"


#include <alloc.h>
#include <ctype.h>


Bool Cross_On = false;


static int driver = DETECT;
static int mode   = 0;
static int xCross, yCross;
static int char_width;
static int char_height;

void pdXGraphics(x) Bool x; {
    if( x ) { char path[100], *trim_at;
	int xasp, yasp;
	if(Graphics) return;

	strcpy(path,first_arg);
	trim_at = strrchr(path, '/');
	if (trim_at == 0) {
	    trim_at = strrchr(path, '\\');
	}
	if (trim_at != 0) {
	    *trim_at = 0;
	}


	detectgraph(&driver,&mode);
	initgraph(&driver,&mode,path);
	Graphics = driver > 0;
	if(!Graphics) {
	    RT_ERROR("Could not load graphics");
	}

	MAXX=getmaxx(); MAXY=getmaxy();
	getaspectratio(&xasp,&yasp);
        XtoYaspect = (double)xasp/yasp;

        setfillstyle(SOLID_FILL, WHITE);

	registerbgifont(small_font);
	settextstyle(SMALL_FONT,HORIZ_DIR, USER_CHAR_SIZE);
        SETCHARSIZE(char_mult);
        settextjustify(LEFT_TEXT,BOTTOM_TEXT);

	setviewport(0,0,MAXX,MAXY,1);

	char_width  = textwidth("X");
	char_height = textheight("X");

        if( Val_Min_X == Val_Max_X ) {
	    xCross = MAXX / 2;
	    yCross = MAXY / 2;

	    Val_Min_X = -1;
	    Val_Max_X =  1;
	    Val_Min_Y = -1;
	    Val_Max_Y =  1;
	    set_scale();
        }
    } else {
	if(!Graphics) return;
	closegraph(); Graphics = false;
    }
}

void OPEN_GRAPH (void){pdXGraphics(true);}
void CLEAR_GRAPH(void){clearviewport();}
void CLOSE_GRAPH(void){pdXGraphics(false);}
void PENSIZE(short width){ setlinestyle(0,0,linewidth=width);}

#define JOT 10

void cross() {
    int left   = max(0,     xCross-JOT);
    int right  = min(MAXX,  xCross+JOT);
    int top    = max(0,     yCross-JOT);
    int bottom = min(MAXY,  yCross+JOT);
    setlinestyle(0,0,NORM_WIDTH);
    setwritemode(XOR_PUT);

    moveto(left,   yCross); lineto(right,yCross);
    moveto(xCross,    top); lineto(xCross,bottom);
    lineto(left, yCross); lineto(xCross,top);
    lineto(right,yCross); lineto(xCross,bottom);

    setlinestyle(0,0,linewidth);
    setwritemode(COPY_PUT);
}

Pd0(pdGetCoord) {
    ENTER;
    Reg(ans);
    Reg(Xval);
    Reg(Yval);
    Reg(Cval);
    char cstring[2];
    int c;
    int last_c = 0;
    float accel = 1;
    char text[50];


    FORCE_GRAPH;

    Cross_On = true;
    cross();
    for(;;){
        char * buffer;
        int left, right, top, bottom, needed, width, height,descent;
	/* display cross position */
	    sprintf(text, "%0.*f, %0.*f",
		x_prec, descaleX(xCross),
                y_prec, descaleY(yCross) );
            /* save and clear region */
                width  = textwidth(text);
                descent = ceil(2*char_mult);
                height = textheight(text)+descent;
                needed = imagesize(0,0,width,height);
                buffer = malloc(needed);
                if( buffer != NULL ) {
		    left    = xCross + ( xCross+width+JOT < MAXX ? JOT
                                                                : -width-JOT);
		    bottom  = yCross + ( yCross-height < 0 ? height
                                                           : 0);
		    right   = left + width;
		    top     = bottom - height;
		    getimage(left,top,right,bottom,buffer);
                    putimage(left,top,buffer, XOR_PUT);

                    setwritemode(COPY_PUT);
	            outtextxy(left,bottom-descent,text);
                }

	c = Egetc();
        /* restore region */
            if( buffer != NULL ) {
                setviewport(0,0,MAXX,MAXY,1);
                putimage(left,top,buffer,COPY_PUT);
                free(buffer);
            }
	cross();
	if( c != 0 ) break;
	c = Egetc();

	/* accelerate if continuing in the same direction */
	    if( c == last_c ) accel *= 1.1; else accel = 1;
	switch( c ) {
	    case UP_ARROW: if( yCross-accel >=   0   ) yCross -= accel; break;
	    case DN_ARROW: if( yCross+accel <= MAXY  ) yCross += accel; break;
	    case LT_ARROW: if( xCross-accel >= 0     ) xCross -= accel; break;
	    case RT_ARROW: if( xCross+accel <= MAXX  ) xCross += accel; break;
	}
	cross();
	last_c = c;
    }

    R(Xval) = New_Real( descaleX(xCross) );
    R(Yval) = New_Real( descaleY(yCross) );
    cstring[0] = c;
    cstring[1] = 0;
    R(Cval) = New_String( cstring );
    R(ans)  = mcCons(R(Cval), Nil);
    R(ans)  = mcCons(R(Yval), R(ans));
    R(ans)  = mcCons(R(Xval), R(ans));
    R(ans)  = New_Tuple(R(ans), def_orig);

    Cross_On = false;
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

void SETCHARSIZE(double x) {
    setusercharsize((int)(x*10),10,(int)(x*10),10);
}

void PREPARE(void) {
}

Pd0(pdXtoYratio) {
    return New_Real(XtoYaspect);
}

#endif
