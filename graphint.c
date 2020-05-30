#ifdef THINK_C
#include <Global.h>
#include <Commands.h>
#include <CApplication.h>
#include <CBartender.h>
#include <CDesktop.h>
#include <CDecorator.h>
#include <CScrollPane.h>
#include <CPanorama.h>
#include <TBUtilities.h>
#include <stdlib.h>
#include "CGraphicsDoc.h"
#include "CGraphicsPane.h"
#include "common.h"

extern CBartender	*gBartender;		/* Manages all menus				*/
extern OSType		gSignature;		/* Creator for Application's files	*/
extern CDesktop		*gDesktop;		/* The visible Desktop				*/
extern CDecorator	*gDecorator;		/* Decorator for arranging windows	*/
extern CApplication	*gApplication;

#include "graphint.h"
#include "graph.h"

CursHandle gCrossCursor, gPlusCursor;
CGraphicsDoc * graphDoc = NULL;
#define TEXTPTS 12

static  textfont = geneva, textface = 0, penmode = COPY_PUT;
static  FontInfo		macFontInfo;


int  opengraph(void) {
    Str255 title = "\pGraphics";
    
    if( Graphics ) return false;
    Graphics = true;
    
    gCrossCursor = GetCursor(crossCursor);
    gPlusCursor = GetCursor(plusCursor);
    SetCursor(*gCrossCursor);
    graphDoc = new(CGraphicsDoc);
    graphDoc->IGraphicsDoc(gApplication);
    graphDoc->NewFile();
    graphDoc->itsWindow->SetTitle(title);
    
    SetPort(graphDoc->itsWindow->macPort);
    TextFont(textfont);
    TextFace(textface);
    char_mult = 1;
    SETCHARSIZE(1);
    linewidth = NORM_WIDTH;
    GetFontInfo(&macFontInfo);	/* Get info about font	*/
   

    return true;
}

int  getmaxx(void) {
    return graphDoc->itsMainPane->frame.right;
}

int  getmaxy(void) {
    return graphDoc->itsMainPane->frame.bottom;
}

void closegraph(void) {
    quick_check();
    if( !Graphics ) return;
    graphDoc->itsWindow->Close();
    graphDoc = NULL;
    Graphics = false;
    quick_check();
}

void set_title(char*x) {
   Str255 temp;
   if( !Graphics ) return;
   CtoPcpy(temp,x);
   graphDoc -> itsWindow -> SetTitle(temp);
}


void GraphicsUpdate(void){
    if( graphDoc != NULL ) { 
        graphDoc ->itsWindow -> Update();
        graphDoc ->itsWindow -> Select();
    }
}


void clearviewport() {
    SUPPRESS;
    EraseRect( &(graphDoc->itsMainPane->frame) );
    DrawGrowIcon( graphDoc->itsWindow->macPort );
    ForeColor(blackColor);
    textfont = courier; textface = 0;
    linewidth = NORM_WIDTH;
    char_mult = 1;
    PenSize(linewidth,linewidth);
    PenMode(patCopy);
    TextFont(textfont);
    TextFace(textface);
    SETCHARSIZE(1);
    TextMode(srcOr);
    MoveTo((int)descaleX(0), (int)descaleY(0));
}
    


void SETCHARSIZE(double x) {TextSize((int)(TEXTPTS*x*MAXX/InitialMax));}
void setwritemode(short mode) {PenMode(mode);}

void rectangle(int left, int top, int right, int bottom){
    Rect R;
    R.left = left; R.top = top; R.right = right; R.bottom = bottom;
    FrameRect( &R );
}
void bar3d(int left, int top, int right, int bottom, int depth, int topoff){
    Rect R;
    R.left = left; R.top = top; R.right = right; R.bottom = bottom;
    PaintRect( &R );
}

void PREPARE(void) {
   if( graphDoc != NULL ) {
       graphDoc->itsWindow->Prepare();
   }
}

Rect textsize(char * text) {
    Rect R;
    Str255 q;

    CtoPcpy(q,text);

    SetPort(graphDoc->itsWindow->macPort);
    TextFont(textfont);
    TextFace(textface);
    SETCHARSIZE(char_mult);


    R.left = 0; R.right = StringWidth(q);
    R.top  = -(macFontInfo.ascent + macFontInfo.leading) ; 
    R.bottom= macFontInfo.descent;
    return R;
}

void ShowCoords(Point * mouse){
    char title[256];
    sprintf(title, "x=%0.*lg  y=%0.*lg -- ", x_prec, descaleX(mouse->h),
                                         y_prec, descaleY(mouse->v) );
    
    PtoCstr((char*)GTitle);
    strncat(title, (char*)GTitle, 255-strlen(title));
    title[255] = 0;
    CtoPstr(title);
    CtoPstr((char*)GTitle);
    graphDoc -> itsWindow -> SetTitle((void*)title);
}

void CtoPcpy(Str255 dest, char * src) {
    strncpy((char*)dest,src, 255);
    dest[255] = 0;
    CtoPstr((char*)dest);
}
#endif
