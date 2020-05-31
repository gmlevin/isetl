#ifdef TURBOC
#ifdef Prototype
#define MENU Menu
#else
#define MENU
#endif

struct menu_pairs { char * text; void (*code)(NONE); };
typedef struct menu_pairs Menu[];


void recenter(NONE);
void touch_all(NONE);
Bool popup(INT X INT X STR);
void save_buff(NONE);
void q_exit(NONE);
void popmenu(MENU);

extern char *dp, *end_dp;


#define WRow(y)      (TOP-1+(y))
#define WCol(x)      (LEFT-1+(x))

#define ctrl(x)      (x & 037)
#define ESC          033
#define UP_ARROW     0110
#define DN_ARROW     0120
#define LT_ARROW     0113
#define RT_ARROW     0115
#define CLT_ARROW    0163
#define CRT_ARROW    0164

#define INS          0122
#define DEL          0123
#define End          0117
#define CEnd         0165
#define PgDn         0121
#define CPgDn        0166
#define Home         0107
#define CHome        0167
#define PgUp         0111
#define CPgUp        (char)0204

#define Fkey(x)      (0072+(x))
#define SFkey(x)     (0123+(x))
#define CFkey(x)     (0135+(x))
#define AFkey(x)     (0147+(x))
#define ZAP          0177

#endif