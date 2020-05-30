#ifdef TURBOC
#include <conio.h>
#define  FORE(x)     ((x)&07)
#define  BACK(x)     (((x)&07)<<4)

#define COLS 80
#define ROWS 23
#define RROW 25
#define LEFT  1
#define TOP   2


void Einit(INT);
void sys(STR);
void special(CHAR);
void bwindow(INT X INT X INT X INT X STR);
Bool popup(INT X INT X STR);
void unpopup(NONE);
void popmsg(INT X STR X STR);
Bool popquery(STR);
FILE*popfile(STR);
char*poptext(INT X STR X STR);
#define NO_FILE_MSG popmsg(0,"Didn't open file.\n"              \
                             "Operation not performed.", NULL);

#endif

char Egetc(NONE);
void Eputc(CHAR);
void Eflush(NONE);
void no_prompt(BOOL);
void q_exit(NONE);
void nl_at_end(NONE);

extern char *prompt_chars;    /* these chars are ignored at the front of a line
                        * when data is sent
                        */

