int  opengraph(void);
int  getmaxx(void), getmaxy(void);
void closegraph(void); 
void set_title(char*);
void GraphicsUpdate(void);
Rect textsize(char * text);
void setwritemode(short mode);
void rectangle(int left, int top, int right, int bottom);
void bar3d(int left, int top, int right, int bottom, int depth, int topoff);
void PREPARE(void);
void StartMouseWatch(void), EndMouseWatch(void), ShowCoords(Point*mouse);
void CtoPcpy(Str255 dest, char * src);

extern CursHandle gCrossCursor, gPlusCursor, gIBeamCursor, gWatchCursor;
extern int InitialMax;
extern int Graphics;
extern int  DataX, DataY, DataC;
extern Str255 GTitle;

#define COPY_PUT patCopy
#define XOR_PUT  patXor
#define THICK_WIDTH 3
#define NORM_WIDTH  1
