#ifdef TURBOC
typedef struct {int left,right,top,bottom;} Rect;
#endif

#ifdef TURBOC
extern Bool Cross_On;
void cross(void);
#endif

#ifdef TURBOC
#undef X
#include <graphics.h>
#endif

/* Machine dependent */
void OPEN_GRAPH(void);         /* initialize graphics */
void CLEAR_GRAPH(void);        /* clear screen */
void CLOSE_GRAPH(void);        /* free graphics */
void PENSIZE(short);           /* set pensize to thick or thin */
void PREPARE(void);

extern Bool Graphics;
extern double X_Mult, Y_Mult, X_Offset, Y_Offset, char_mult, XtoYaspect;
extern double Val_Min_X, Val_Min_Y, Val_Max_X, Val_Max_Y;
extern int  linewidth;
extern int  x_prec, y_prec;

extern int  MAXX, MAXY;
extern int  DataX, DataY, DataC;
extern int  CPx, CPy, InitialMax;



/* Machine independent */
void START_GRAPH(void);
void STOP_GRAPH(void);
void OUTTEXT(char*);
void MOVETO(double px, double py);
void LINETO(double px, double py);
int  NEW_OBJECT(void);                /* Start a new object */
void DEL_OBJECT(int);                 /* Remove obj from image */
void ADD_OBJECT(int);                 /* Add obj to image */
void CHAR_MULT(double x);
void SAVE_GRAPH(char * x);
void LOAD_GRAPH(char * x);
void ADD_POINT(double a, double b, short c);
void ADD_RECTANGLE(double L, double T, double R, double B, Bool hollow);

void refreshscreen(void);
void erase_it(void);
void erase_start(void);
void erase_stop(void);
void flip_mode(void);
void set_scale(void);


/* slight machine dependence */
void SOUTTEXT(char*);
void SMOVETO(double px, double py);
void SLINETO(double px, double py);
void SPOINT(double px, double py, short type);
void SMULT(double x);
void SRECTANGLE(double L, double T, double R, double B, Bool hollow);
void SETLINEWIDTH(short);
void SETCHARSIZE(double char_mult);



#ifndef max
#define min(a,b)   ((a)<(b) ? a : b)
#define max(a,b)   ((a)>(b) ? a : b)
#endif

#define scaleX(px)    (int)(px*X_Mult+X_Offset)
#define scaleY(py)    (int)(-py*Y_Mult+Y_Offset)
#define descaleX(px)  ( (px-X_Offset)/X_Mult )
#define descaleY(py)  ( (py-Y_Offset)/-Y_Mult )

#ifdef TURBOC
#define SUPPRESS       if( !Graphics ) return; else
#define FORCE_GRAPH    if( !Graphics ) flip_mode(); else
#endif
#ifdef Mac
#define SUPPRESS       if( !Graphics ) return; else PREPARE()
#define FORCE_GRAPH    if( !Graphics ) return; else PREPARE()
#endif
