#include "common.h"
#ifdef Screen
#ifdef TURBOC

#include "buffer.h"
#include "screen.h"
#include <alloc.h>
#include <assert.h>

EBuf current_buffer = NULL;


/* allocate a new buffer */
EBuf new_buffer(size) int size; {
    EBuf new = malloc( sizeof *new );
    if( new == NULL ) { return NULL; }
    new->def_mark = new->last_front = new->last_back
        = new->wstart = new->cursor = new->start = malloc(size);
    if( new->start == NULL ) { free(new); return NULL; }
    *(new->start) = EOS;
    new->gap      = new->start+1;
    new->tail     = new->stop = new->start + size;
    new->goal_col = -1;
    return new;
}

void reset_buffer() {
    EBuf new = current_buffer;
    new->def_mark = new->last_front = new->last_back
	= new->wstart = new->cursor = new->start;
    *(new->start) = EOS;
    new->gap      = new->start+1;
    new->tail     = new->stop;
    new->goal_col = -1;
}

/* advance cursor.  indicate success */
Bool cur_inc() {
    CURSOR ++;
    if( CURSOR == GAP ) {  /* need to shift gap down some */
        if( !gap_dn() ) {  /* can't move cursor (no chars follow) */
            CURSOR --;
            return false;
        } else {
           win_dn();
        }
    }
    return true;
}


Bool cur_dec() {
    if( CURSOR == START ) return false;

    if( CURSOR == WSTART ){ win_up(); gap_up(); }
    CURSOR--;
    return true;
}



/* insert c before CURSOR */
void cur_ins(c) char c; {
    char * p;
    if( GAP == TAIL ) {  /* scroll top 1/3 out of buffer */
        int skip = (STOP-START) / 3;
        char *from, *to;

        from = START+skip;
        to   = START;

        if( popquery("Buffer full.\nSave overflow to a file?") ){
            FILE * f = popfile("a");
            char * p;
            if( f==NULL ) {
                NO_FILE_MSG;
            } else {
                for( p = to; p < from; p++ ) putc(*p, f);
                fclose(f);
            }
        }


        /* trouble.  what do we do? */
            if(CURSOR < from || (dp != NULL && dp < from )) {
                while( true ) {
                    popmsg(0, "Cursor or region in overflow from buffer.\n"
                              "You MAY save the buffer.  You MUST quit.",
                               NULL );
                    save_buff();
                    q_exit();
                }
            }
	    if( DEF_MARK   < from ) DEF_MARK   = CURSOR;
            if( LAST_FRONT < from ) {
                LAST_FRONT = CURSOR;
                LAST_BACK  = CURSOR;
            }

        while( from < GAP ) {
            if( CURSOR     == from ) CURSOR     = to;
            if( DEF_MARK   == from ) DEF_MARK   = to;
            if( LAST_FRONT == from ) LAST_FRONT = to;
            if( LAST_BACK  == from ) LAST_BACK  = to;
            if(               dp     == from ) dp        = to;
            if( dp != NULL && end_dp == from ) end_dp    = to;
            *(to++) = *(from++);
        }
        GAP = to;
        unpopup();
        WSTART = cur_first(CURSOR);
        touch_all();
        recenter();
    }
    if( CURSOR <= DEF_MARK   &&  DEF_MARK  < GAP ) DEF_MARK++;
    if( CURSOR <= LAST_FRONT && LAST_FRONT < GAP ) LAST_FRONT++;
    if( CURSOR <= LAST_BACK  && LAST_BACK  < GAP ) LAST_BACK++;

    for( p=GAP; p>CURSOR; p-- ){
        *p = *(p-1);
    }
    *(CURSOR) = c;
    CURSOR ++;
    GAP ++;
}

/* delete char before CURSOR */
char cur_del() {
    char * p, c;
    if( CURSOR == START ) { return 0; }
    if( CURSOR <= DEF_MARK   &&  DEF_MARK  < GAP ) DEF_MARK--;
    if( CURSOR <= LAST_FRONT && LAST_FRONT < GAP ) LAST_FRONT--;
    if( CURSOR <= LAST_BACK  && LAST_BACK  < GAP ) LAST_BACK--;

    c = *(CURSOR-1);
    for( p=CURSOR; p<GAP; p++ ){
        *(p-1) = *p;
    }
    CURSOR --;
    GAP --;
    return c;
}

/* move cursor to next line.  aim for col, settle for EOLN if short.
 * return col actually in, -1 if no next line.
 */
int cur_dn(col) int col; {
    int i;
    char * old_cursor = CURSOR;   /* usable even after cur_inc.
                                   * gap only increases here.
                                   */

    /* go to end of line */
        while( cur_char != '\n' && cur_inc() ) ;

        if( cur_char != '\n' ) {
            CURSOR = old_cursor;
            return -1;
        }

    /* start next line */
        cur_inc();

    /* aim for col */
        i = 1;
        while( i < col && cur_char!='\n' && cur_inc()) {
            i++;
        }
    return i;
}


/* move cursor to prev line.  aim for col, settle for EOLN if short.
 * return col actually in, -1 if no prev line.
 */
int cur_up(col) int col; {
    int i;
    char * old_cursor = CURSOR;   /* usable even after cur_dec.
                                   * gap unchanged.
                                   */

    /* go to end of prev line */
        if( !cur_dec() ) { /* nothing prev */
            return -1;
        }
        while( cur_char != '\n' && cur_dec() ) ;

        if( cur_char != '\n' ) { /* no prev line */
            CURSOR = old_cursor;
            return -1;
        }

    /* go to beginning of prev line */
        while( START < CURSOR && prev_char != '\n')  cur_dec();

    /* aim for col */
        i = 1;
        while( i < col && cur_char!='\n' && cur_inc()) {
            i++;
        }
    return i;
}

void cur_home(){
    while( START < CURSOR && prev_char != '\n' ) cur_dec();
}

/* advance cursor to end of line, return # of chars skipped */
int cur_end() {
    int i;
    i = 0;
    while( cur_char != '\n' && cur_inc() ) i++;
    return i;
}

/* pointer to last char of line containing p */
/* assumes line not broken by gap */
char * cur_last(p) char * p;{
    if( p == NULL ) return p;
    while( *p != '\n' && *p != EOS ) p++;
    return p;
}


/* pointer to first char of line containing p */
char * cur_first(p) char * p;{
    if( p == NULL ) return p;
    while( p != START && p != TAIL && *(p-1) != '\n' ) p--;
    return p;
}

/* return false if at end of buffer */
Bool gap_dn() {
    char c;
    if( TAIL == STOP ) return false;
    do {
        if( LAST_FRONT == TAIL ) LAST_FRONT = GAP;
        if( LAST_BACK  == TAIL ) LAST_BACK  = GAP;
        if(  DEF_MARK  == TAIL ) DEF_MARK   = GAP;
        if(    CURSOR  == TAIL ) CURSOR     = GAP;
    } while( (c =(*(GAP++) = *(TAIL++))) != '\n' && c != EOS );

    return true;
}

/* Should not be called when at the top of the window */
Bool gap_up() {
    do {
        *(--TAIL) = *(--GAP);
        if( LAST_FRONT == GAP ) LAST_FRONT = TAIL;
        if( LAST_BACK  == GAP ) LAST_BACK  = TAIL;
        if(  DEF_MARK  == GAP ) DEF_MARK   = TAIL;
    } while( *(GAP-1) != '\n' );
    return true;
}


/* Should not be called at the end of the buffer */
Bool win_dn() {
    char c;
    while( (c =*(WSTART++)) != '\n' && c != EOS ) ;
    return true;
}

/* Return false if at top of buffer */
Bool win_up() {
    if( WSTART == START ) return false;
    --WSTART; /* enter previous line */
    if( WSTART == START ) return true;
    while( WSTART > START && *(WSTART-1) != '\n' ) WSTART--;
    return true;
}

/* Number of lines between WSTART and GAP */
int line_count(){
    char * p;
    int lines = 0;
    for( p = WSTART; p < GAP; p++ ) if( *p == '\n' ) lines++;
    if( *(GAP-1) != '\n' ) lines++;
    return lines;
}

void line_del() {
    char * front = cur_first(CURSOR);
    char * end   = cur_last(CURSOR);
    int delta;
    if( *end == '\n' ) { end++; }
    delta = end - front;

    CURSOR = front;

    DEF_MARK = cur_first(DEF_MARK);
    if( CURSOR < DEF_MARK   &&  DEF_MARK  < GAP ) DEF_MARK   -= delta;

    LAST_FRONT = cur_first(LAST_FRONT);
    if( CURSOR < LAST_FRONT && LAST_FRONT < GAP ) LAST_FRONT -= delta;

    LAST_BACK = cur_first(LAST_BACK);
    if( CURSOR < LAST_BACK  && LAST_BACK  < GAP ) LAST_BACK  -= delta;

    for( ; end != GAP ; front++, end++ ) {
	*front = *end;
    }
    GAP -= delta;
    gap_dn();

}

#endif
#endif
