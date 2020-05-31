struct edit_buffer {
    char *start, *wstart, *cursor, *gap, *tail, *stop;
    char *def_mark, *last_front, *last_back;
    int b_rows, b_cols;
    int goal_col;
};


/* start <= wstart <= cursor < gap <= tail <= stop
   data stored in [start..gap),[tail..stop)
   start points to the memory allocated.

   stop points to the first byte after the memory allocated.
   cursor points to the character before which insertion would take place.
   Last char is always EOS.  This provides a character to insert before.
   Lines are never broken by gap.

   wstart points to the first char of window.
   window ends at gap.

   goal_col == 0 indicates no goal set (used for go_up and go_dn).
*/

typedef struct edit_buffer * EBuf;

extern EBuf current_buffer;

#define START   (current_buffer->start)
#define WSTART  (current_buffer->wstart)
#define CURSOR  (current_buffer->cursor)
#define GAP     (current_buffer->gap)
#define TAIL    (current_buffer->tail)
#define STOP    (current_buffer->stop)
#define GOAL_COL        (current_buffer->goal_col)
#define DEF_MARK        (current_buffer->def_mark)
#define LAST_FRONT      (current_buffer->last_front)
#define LAST_BACK       (current_buffer->last_back)
#define B_ROWS  (current_buffer->b_rows)
#define B_COLS  (current_buffer->b_cols)


#define cur_char        (*(CURSOR))
#define prev_char       (*(CURSOR-1))


EBuf new_buffer(INT);   /* allocate a new buffer */
void reset_buffer(NONE);/* reset buffer */
Bool cur_inc(NONE);     /* advance cursor.  indicate success */
Bool cur_dec(NONE);     /* decrement cursor.  indicate success */
void cur_ins(CHAR);     /* insert c before CURSOR */
char cur_del(NONE);     /* delete and return char before CURSOR */
void line_del(NONE);	/* delete current line */
int  cur_dn(INT);       /* cursor down, aim for col */
int  cur_up(INT);       /* cursor up, aim for col */
void cur_home(NONE);    /* back up cursor to front of line */
int  cur_end(NONE);     /* advance cursor to end of line (= # skipped) */
char*cur_first(STR);    /* pointer to first char in line */
char*cur_last(STR);     /* pointer to last char ('\n') in line */
Bool gap_up(NONE);      /* send a line below the gap */
Bool gap_dn(NONE);      /* bring a line up from below the gap */
Bool win_dn(NONE);      /* Should not be called at the end of the buffer */
Bool win_up(NONE);      /* Return false if at top of buffer */
int  line_count(NONE);  /* Number of lines between WSTART and GAP */
