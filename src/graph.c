#include "common.h"
#ifdef GRAPHICS
#include "manager.h"
#include "predef.h"
#include "micro.h"
#include "graph.h"
#ifdef Mac
#    define moveto MoveTo
#    define lineto LineTo
#    include "graphint.h"
#endif

#include <time.h>
#ifdef Mac
#define CLK_TCK CLOCKS_PER_SEC
#endif

int  DataX=-1, DataY=-1, DataC = -1, InitialMax, CPx, CPy;
double X_Mult=1, Y_Mult=1, X_Offset=0, Y_Offset=0, XtoYaspect;
double char_mult=1;
double Val_Min_X=0, Val_Min_Y, Val_Max_X=0, Val_Max_Y;
Bool Graphics = false;
int  x_prec, y_prec;
int  MAXX, MAXY;

enum actions {Aline, Amove, Asize, Atext, Amult, Apoint, Arect1, Arect2};

typedef struct { double x,y; short type;} APt;

typedef  struct ACTS *Acts;
struct ACTS{
    Acts next;
    enum actions do_it;
    union{
        short data;
        char *str;
        APt P;
    } parts;
};

#define DO_IT(p)     ((p)->do_it)
#define DATA(p)      ((p)->parts.data)
#define TEXT(p)      ((p)->parts.str)
#define P(p)         ((p)->parts.P)
#define NEXT(p)      ((p)->next)


typedef struct Node *Link;
struct Node{
    Link next;
    int  obj;
};

static obj_count = 0;
typedef struct QueueList *QLink;
struct QueueList {
    QLink next;
    Acts head,tail;
};

static Bool  Erase_Mode = false;
static Link  Image   = NULL;   /* List of object indexes */
static QLink Objects = NULL;   /* List of defined objects (origin 0) */
                               /* Each object is a queue of Acts */
static QLink Current = NULL;   /* Last object in Objects */
static struct QueueList  QErase
       = {0,0,0};              /* Erasable acts, written in XOR mode */
static QLink Erasable = &QErase;

C_Real Get_Real(IPtr);
void add_act(enum actions d, short v, double px, double py, char*s);
void do_act (Acts p);
void draw_erasable(void);
void draw_obj(int obj);
void clear_image(void);
void clear_erasable(void);
void clear_objects(void);
void *NEW(short size);

void *NEW(short size) {
    void * new = malloc(size);
    if( new==NULL ) { clear_objects(); RT_ERROR("Out of graphics space"); }
    NEXT((Acts)new) = NULL;
    return new;
}

void clear_erasable(void) {
    Acts p, pp;

    p = Erasable->head;
    while( p!=NULL ) {
	pp = p;
	p  = NEXT(p);
	/* free action pp */
	    if( DO_IT(pp) == Atext ){ free(TEXT(pp)); }
	    free(pp);
    }
    Erasable->head = Erasable->tail = NEW(sizeof*(Erasable->head));
    DO_IT(Erasable->head)= Asize;
    DATA(Erasable->head) = linewidth;
    Erase_Mode = false;
}

void clear_image(void) {
    Link t;
    while( Image != NULL ) {
        t = Image;
        Image = NEXT(Image);
        free(t);
    }
    Image = NEW(sizeof*Image);
    Image->obj  = obj_count;
}

void clear_objects(void){
    Acts p, pp;
    QLink q;
    static Bool clearing = false;  /* Prevent clearing due to out of memory */

    if( clearing ) {FATAL("No free memory for graphics"); }
    clearing = true;

    obj_count   = 0;
    clear_image();

    while(Objects != NULL) {
        q = Objects;
        Objects = NEXT(Objects);
        p = q->head;
        while( p!=NULL ) {
            pp = p;
            p  = NEXT(p);
            /* free action pp */
                if( DO_IT(pp) == Atext ){ free(TEXT(pp)); }
                free(pp);
        }
        free(q);
    }
    Objects = NEW(sizeof*Objects);
    Objects->head = Objects->tail = NEW(sizeof*(Objects->head));
    Current = Objects;
    DO_IT(Current->head)= Asize;
    DATA(Current->head) = linewidth;
    add_act(Amult, 0, char_mult, 0, 0);
    add_act(Asize, linewidth, 0,0,0);
    add_act(Amove, 0, 0,0, 0);


    clear_erasable();
    clearing = false;
}

void START_GRAPH(void) {
    Val_Min_X = Val_Max_X = 0;  /* Will cause rescaling */
    OPEN_GRAPH();
    clear_objects();
}

void STOP_GRAPH(void) {
    clear_objects();
#ifdef TURBOC
    if( Graphics ) flip_mode();
#endif
    CLOSE_GRAPH();
}

void OUTTEXT(char * text)        {    add_act(Atext,0,0,0,text); }
void MOVETO(double px, double py){    add_act(Amove,0,px,py,0);}
void LINETO(double px, double py){    add_act(Aline,0,px,py,0);}
void SETLINEWIDTH(short linewidth){   add_act(Asize, linewidth, 0,0,0);}
void CHAR_MULT(double x) {            add_act(Amult, 0, x, 0, 0); }
void ADD_POINT(double a, double b, short c) {add_act(Apoint, c,a,b,0); }
void ADD_RECTANGLE(double L, double T, double R, double B, Bool hollow){
    add_act(Arect1,hollow,L,T,0);
    add_act(Arect2,    0 ,R,B,0);
}

int  NEW_OBJECT(void){
    Link ITail;

    ++obj_count;

    /* attach new obj to end of object list */
    NEXT(Current) = NEW(sizeof*Current);
    Current       = NEXT(Current);

    /* make current object empty */
    Current->head = Current->tail = NEW(sizeof*(Current->head));
    DO_IT(Current->head)= Asize;
    DATA(Current->head) = linewidth;

    add_act(Amult, 0, char_mult, 0, 0);
    add_act(Asize, linewidth, 0,0,0);
    add_act(Amove, 0, descaleX(CPx),descaleY(CPy), 0);

    /* add current object to image */
    for( ITail=Image; NEXT(ITail)!=NULL; ITail=NEXT(ITail) ) ;

    NEXT(ITail) = NEW(sizeof*ITail);
    ITail = NEXT(ITail);
    ITail->obj  = obj_count;

    return obj_count;
}

/* Remove obj from image */
void DEL_OBJECT(int obj) {
    Link p;

    for( p=Image; NEXT(p)!=NULL; p=NEXT(p) ){
        if( NEXT(p)->obj == obj ) {
            Link t = NEXT(p);
            NEXT(p) = NEXT(t);
            free(t);
            CLEAR_GRAPH();
            refreshscreen();
            return;
        }
    }
}

/* Add obj to image */
void ADD_OBJECT(int obj) {
    Link p;

    for( p=Image; NEXT(p)!=NULL; p=NEXT(p) ){
        if( NEXT(p)->obj == obj ) {
            return;
        }
    }
    NEXT(p) = NEW(sizeof*NEXT(p));
    p = NEXT(p);
    p->obj = obj;
    SUPPRESS;
    draw_erasable();
    draw_obj(obj);
    draw_erasable();
}


static void add_act(enum actions d, short v, double px, double py, char*s){
    Acts new = NEW(sizeof *new);
    QLink active = Erase_Mode ? Erasable : Current;
    Acts prev;

    /* Add action to end of active */
	DO_IT(new) = d;
        switch(d) {
          case Arect2:
	  case Aline: case Amove:     P(new).x = px; P(new).y = py; break;
	  case Asize:                 DATA(new) = v; break;
          case Arect1:
          case Apoint:                P(new).x = px; P(new).y = py; 
                                      P(new).type = v; break;
          case Amult:                 P(new).x = px;   break;
	  case Atext: TEXT(new) = malloc(strlen(s)+1);
                      if( TEXT(new) == NULL ) {
                          clear_objects();
                          RT_ERROR("Out of graphics space");
                      }
                      strcpy(TEXT(new),s); break;
          default: assert(0);
        }

        NEXT(active->tail) = new;
        prev = active->tail;
        active->tail = new;

    if( d==Arect1 ) return;
#ifdef TURBOC
    FORCE_GRAPH;
#endif
#ifdef Mac
    SUPPRESS;
#endif
    if(!Erase_Mode) draw_erasable(); else setwritemode(XOR_PUT);
    do_act(d==Arect2 ? prev : new);
    if(!Erase_Mode) draw_erasable(); else setwritemode(COPY_PUT);
}

void refreshscreen() {
    Link t;

    for( t=Image; t != NULL; t=NEXT(t) ) {
        draw_obj(t->obj);
    }
    draw_erasable();
#ifdef TURBOC
    if( Cross_On ) cross();
#endif
}



void SOUTTEXT(char* text) {
#ifdef Mac
    Str255 temp;
    Rect R;
    CtoPcpy(temp,text);
    DrawString(temp);
    R = textsize(text);
    CPx = CPx + R.right;
#endif
#ifdef TURBOC
    outtext(text);
    CPx = getx();
    CPy = gety();
#endif
}


void SMOVETO(double px, double py) {
    CPx = scaleX(px);
    CPy = scaleY(py);
    moveto(CPx,CPy);
}

void SLINETO(double px, double py) {
    CPx = scaleX(px);
    CPy = scaleY(py);
    lineto(CPx,CPy);
}


void SPOINT(double px, double py, short type) {
    CPx = scaleX(px);
    CPy = scaleY(py);

    if( type==0 ) {
        moveto(CPx,CPy);
        lineto(CPx,CPy);
    } else if( type > 0 ) {
        rectangle(CPx-type,CPy-type*XtoYaspect,CPx+type,CPy+type*XtoYaspect);
    } else {
        bar3d(CPx+type,CPy+type*XtoYaspect,CPx-type,CPy-type*XtoYaspect,0,0);
    }
    moveto(CPx,CPy);
}

void SMULT(double x) {
    char_mult = x; 
    SETCHARSIZE(char_mult);
}

void SRECTANGLE(double L, double T, double R, double B, Bool hollow) {
    if( hollow ) {
        rectangle(scaleX(L), scaleY(T), scaleX(R), scaleY(B));
    } else {
        bar3d(scaleX(L), scaleY(T), scaleX(R), scaleY(B),0,0);
    }
    CPx = scaleX(R); CPy = scaleY(B);
}




void do_act (Acts p) {
    switch( DO_IT(p) ) {
      case Aline : SLINETO(P(p).x, P(p).y);  break;
      case Amove : SMOVETO(P(p).x, P(p).y);  break;
      case Asize : PENSIZE(DATA(p));         break;
      case Atext : SOUTTEXT(TEXT(p));        break;
      case Apoint: SPOINT(P(p).x, P(p).y, P(p).type);  break;
      case Amult : SMULT(P(p).x);            break;
      case Arect1: SRECTANGLE(P(p).x,P(p).y,
			      P(NEXT(p)).x,P(NEXT(p)).y,P(p).type);
                   break;
      case Arect2: break;
      default: assert(0);
    }
}

void draw_erasable(void) {
    Acts p;
    int was_x = CPx;
    int was_y = CPy;
    int was_linewidth = linewidth;
    setwritemode(XOR_PUT);

    for( p = Erasable->head; p!=NULL; p=NEXT(p) ) {
	do_act(p);
    }
    setwritemode(COPY_PUT);
    PENSIZE(was_linewidth);
    moveto(was_x,was_y);
}


void erase_it(void) {
    PREPARE();
    if( Graphics ) draw_erasable();
    clear_erasable();
}

void erase_start(void) {
    Erase_Mode = true;
    add_act(Asize, linewidth, 0,0,0);
    add_act(Amult, 0, char_mult,0,0);
    add_act(Amove, 0, descaleX(CPx),descaleY(CPy), 0);
}

void erase_stop(void) {
    Erase_Mode = false;
}


static void draw_obj(int obj){
    QLink q;
    Acts p;
    int i;

    for( q=Objects, i=0; i!=obj && q != NULL; i++, q=NEXT(q)) ;

    if( q == NULL ) return;
    for( p = q->head; p!=NULL; p=NEXT(p) ) {
	do_act(p);
    }
}


static void write_obj(FILE * F, int obj) {
    QLink q;
    Acts p;
    int i;

    if( obj==-1 ) q = Erasable;
    else  for( q=Objects, i=0; i!=obj && q != NULL; i++, q=NEXT(q)) ;

    if( q == NULL ) return;
    for( p = q->head; p!=NULL; p=NEXT(p) ) {
	switch( DO_IT(p) ) {
          case Arect2:
	  case Aline :
	  case Amove : fprintf(F,"%d %g %g\n", DO_IT(p), P(p).x, P(p).y); break;
	  case Asize : fprintf(F,"%d %d\n", DO_IT(p), DATA(p)); break;
	  case Atext : fprintf(F,"%d `%s`\n", DO_IT(p), TEXT(p)); break;
          case Arect1:
	  case Apoint: fprintf(F,"%d %g %g %d\n", DO_IT(p), P(p).x, P(p).y, P(p).type); break;
	  case Amult : fprintf(F,"%d %g\n", DO_IT(p), P(p).x); break;
	  default: assert(0);
	}
    }
}

void SAVE_GRAPH(char * filename) {
    Link t;
    FILE * F;

    if( filename==NULL || filename[0]==EOS ) {
#ifdef TURBOC
        F = popfile("w"); /* need mac version */
#endif
#ifdef Mac
        RT_ERROR("No filename.  Use SAVE in menu.");
#endif
    } else {
        F = FOpen(filename, "w");
    }
    if( F==NULL ) {
        RT_ERROR("save_graph: could not open file");
    }

    for( t=Image; t != NULL; t=NEXT(t) ) {
        write_obj(F, t->obj);
    }
    write_obj(F, -1);
    fclose(F);
}

#define MAX_LINE 200

void LOAD_GRAPH(char * filename) {
    FILE * F;
    int d;
    double px, py;
    int v;
    char text[MAX_LINE];
    int c;

    if( filename==NULL || filename[0]==EOS ) {
#ifdef TURBOC
        F = popfile("r"); /* need mac version */
#endif
#ifdef Mac
        RT_ERROR("load_graph needs a file name");
#endif
    } else {
        F = FOpen(filename, "r");
    }
    if( F==NULL ) {
        RT_ERROR("load_graph: could not open file");
    }

    while( (c=fscanf(F, "%d", &d)) != 0  && c != EOF ) {
	switch( d ) {
          case Arect2:
	  case Aline :
	  case Amove : fscanf(F," %lg %lg\n",    &px, &py); break;
	  case Asize : fscanf(F," %d\n",         &v); break;
	  case Atext : fscanf(F," `%[^`]`\n",    &text); break;
          case Arect1:
	  case Apoint: fscanf(F," %lg %lg %d\n", &px, &py, &v); break;
	  case Amult : fscanf(F," %lg\n",        &px); break;
	  default: assert(0);
	}
        add_act(d,v,px,py,text);
    }
    fclose(F);
}


/* Predefs */
Pd1(pdCharMult) {
    double old = char_mult;
    CHAR_MULT(Get_Real(x));
    return New_Real(old);
}

Pd1(pdSaveGraph) {
    if( Kind(x) != String ) { 
        RT_ERROR("save_graph(filename)");
    }
    SAVE_GRAPH(Str_Value(x));
    return NonPrint;
}

Pd1(pdLoadGraph) {
    if( Kind(x) != String ) { 
        RT_ERROR("load_graph(filename)");
    }
    LOAD_GRAPH(Str_Value(x));
    return NonPrint;
}

Pd(pdPoint) {
    IPtr a,b,c;
    Bool SingleArg;
    
    Pd_Arg(a); Pd_Arg(b); Pd_Arg(c);
    SingleArg = b==OM && c==OM;
    
    if( Kind( SingleArg ? a : c) != Integer ) {
        RT_ERROR("point(x,y,type)");
    }
    if( SingleArg )
         ADD_POINT(descaleX(CPx), descaleY(CPy), Int_Value(a));
    else ADD_POINT(Get_Real(a), Get_Real(b), Int_Value(c));
    return NonPrint;
}

Pd1(pdAddObj) {
    if( Kind(x) != Integer ) {
        RT_ERROR("add_object(int)");
    }
    ADD_OBJECT(Int_Value(x));
    return NonPrint;
}

Pd1(pdDelObj) {
    if( Kind(x) != Integer ) {
        RT_ERROR("del_object(int)");
    }
    DEL_OBJECT(Int_Value(x));
    return NonPrint;
}

Pd0(pdNewObj) {
    int i = NEW_OBJECT();
    return New_Integer(i);
}

Pd(pdErasable){
    IPtr effect;

    Pd_Arg(effect);

    if(      effect==OM    ) erase_it();
    else if( effect==ITRUE ) erase_start();
    else if( effect==IFALSE) erase_stop();
    else RT_ERROR("Bad argument to 'erasable'");
    return NonPrint;
}

Pd1(pdTitleW){
    int PlotX, PlotY;
    if(Kind(x) != String) {RT_ERROR("title_window needs string");}

    FORCE_GRAPH;
#ifdef TURBOC
    PlotX = descaleX(CPx);
    PlotY = descaleY(CPy);

    MOVETO(Val_Min_X+(Val_Max_X-Val_Min_X)*0.05,
           Val_Max_Y-(Val_Max_Y-Val_Min_Y)*0.1);
    OUTTEXT(Str_Value(x));
    
    MOVETO(PlotX,PlotY);
#endif
#ifdef Mac
    set_title(Str_Value(x));
#endif
    return NonPrint;
}

int linewidth =  NORM_WIDTH;

Pd1(pdThickLine) {
    int was_width = linewidth;

    if( Kind(x) != Boolean ) {
        RT_ERROR("thickline(boolean)");
    }
    SETLINEWIDTH(x==ITRUE ? THICK_WIDTH : NORM_WIDTH);
    return (was_width==THICK_WIDTH ? ITRUE : IFALSE);
}

Pd1(pdGraphics){
    if(      x==ITRUE ) START_GRAPH();
    else if( x==IFALSE) STOP_GRAPH();
    else RT_ERROR("graphics(BOOL)");
    return NonPrint;
}

static C_Real Get_Real(x) IPtr x; {
    C_Real r;
    switch(Kind(x)){
      case Real: case Rational:
	case Bignum: case Integer: r = Real_Value(mpFloat(x)); break;
	default:  RT_ERROR("Number needed as arg to graphics routine");
    }
    return r;
}

Pd2(pdMove){
    MOVETO(Get_Real(a),Get_Real(b));
    return NonPrint;
}

Pd2(pdDraw){
    LINETO(Get_Real(a),Get_Real(b));
    return NonPrint;
}

Pd(pdTextout){
    IPtr x,y,text;

    Pd_Arg(x); Pd_Arg(y); Pd_Arg(text);
    if( Kind(x)==String && y==OM && text==OM ) {
        /* Print at CP */
        OUTTEXT( Str_Value(x) );
    } else {
        if( Kind(text) != String ) { 
            RT_ERROR("textout(x,y,string) or textout(string)");
        }
        MOVETO(Get_Real(x),Get_Real(y));
        OUTTEXT( Str_Value(text) );
    }
    return NonPrint;
}

void set_scale(void) {
    double RCPx, RCPy;

    if( Val_Min_X >= Val_Max_X || Val_Min_Y >= Val_Max_Y ) {
        RT_ERROR("Attempt to set improper scale");
    }

    FORCE_GRAPH;

    RCPx = descaleX(CPx);
    RCPy = descaleY(CPy);    

    MAXX = getmaxx(); MAXY=getmaxy();

    x_prec = ceil(-log10(Val_Max_X-Val_Min_X))+2;
    if( x_prec < 2 ) x_prec= 2;

    y_prec = ceil(-log10(Val_Max_Y-Val_Min_Y))+2;
    if( y_prec < 2 ) y_prec= 2;

    X_Mult = MAXX  / (Val_Max_X - Val_Min_X);
    Y_Mult = MAXY / (Val_Max_Y - Val_Min_Y);
    X_Offset = -Val_Min_X * X_Mult;
    Y_Offset =  Val_Min_Y * Y_Mult + MAXY;
    DataX = 0;
    DataY = 0;
    
    CPx = scaleX(RCPx);
    CPy = scaleY(RCPy);
}


Pd(pdScale){
    IPtr min_x, min_y, max_x, max_y;

    Pd_Arg(min_x);    Pd_Arg(max_x);    Pd_Arg(min_y);    Pd_Arg(max_y);
    Val_Min_X = Get_Real(min_x);
    Val_Max_X = Get_Real(max_x);
    Val_Min_Y = Get_Real(min_y);
    Val_Max_Y = Get_Real(max_y);

    set_scale();
    CLEAR_GRAPH();
    refreshscreen();
    return NonPrint;
}

Pd0(pdClearScreen) {
    clear_objects();
    CLEAR_GRAPH();
    refreshscreen();
    return NonPrint;
}

Pd(pdRectangle) {
    IPtr L,T,R,B, hollow;
    Pd_Arg(L); Pd_Arg(R); Pd_Arg(B); Pd_Arg(T); Pd_Arg(hollow);

    if( T==OM ) {
        RT_ERROR("rectangle: too few args");
    }
    if( Kind(hollow)!=Boolean && hollow!=OM ) {
        RT_ERROR("rectangle(left,top,bottom,right, hollow)");
    }
    ADD_RECTANGLE(Get_Real(L), Get_Real(T), Get_Real(R), Get_Real(B),
	      hollow==OM || hollow==ITRUE );
    return NonPrint;
}

Pd0(pdWhere) {
    ENTER;
    Reg(ans);

    R(ans) = Nil;
    R(ans) = mcCons(OM, R(ans));
    Info(R(ans)) = New_Real(descaleY(CPy));
    R(ans) = mcCons(OM, R(ans));
    Info(R(ans)) = New_Real(descaleX(CPx));

    R(ans) = New_Tuple(R(ans),1);

    LEAVE R(ans);
}

Pd1(pdDelay) {
    C_Real t = Get_Real(x);
    clock_t stop, c;
    
    stop = clock() + t*CLK_TCK;
    while( (c=clock())<stop && c!=(clock_t)-1 ) ;
    return NonPrint;
}
#endif

