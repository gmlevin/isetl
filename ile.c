#include "common.h"

#ifndef Screen
void    delete_char_under(CHAR);
void    delete_char(CHAR);
void    quote(CHAR);
void	escape_1(CHAR);
void	escape_2(CHAR);
void	escape_3(CHAR);
void    start_of_line(CHAR);
void    end_of_line(CHAR);
void    backward_char(CHAR);
void    forward_char(CHAR);
void    add_to_history(CHAR);
void    erase_line(CHAR);
void    erase_to_end_of_line(CHAR);
void    retype_line(CHAR);
void    forward_history(CHAR);
void    backward_history(CHAR);
void    sig_int(CHAR);



#	ifdef TURBOC
#	include <conio.h>
#	endif

/*
 * This has been extensively trimmed by Gary Levin while at
 * Clarkson University.
 * You should contact the original author for the full version, which was
 * originally intended for Unix.
 */


/*
 * COPYRIGHT 1988 Evans & Sutherland Computer Corporation Salt Lake City,
 * Utah All Rights Reserved.
 *
 * THE INFORMATION  IN  THIS  SOFTWARE  IS  SUBJECT  TO  CHANGE WITHOUT
 * NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY  EVANS  &
 * SUTHERLAND. EVANS  &  SUTHERLAND   MAKES  NO REPRESENTATIONS  ABOUT
 * THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  IT IS SUPPLIED  "AS
 * IS"  WITHOUT EXPRESS  OR IMPLIED WARRANTY.
 *
 * IF THE SOFTWARE IS MODIFIED IN A MANNER CREATING  DERIVATIVE COPYRIGHT
 * RIGHTS,  APPROPRIATE LEGENDS MAY BE PLACED ON THE DERIVATIVE WORK IN
 * ADDITION TO THAT SET FORTH ABOVE.
 *
 * Permission  to  use,  copy,  modify,  and  distribute   this software  and
 * its documentation for any purpose and without fee is hereby granted,
 * provided  that  the  above  copyright notice  appear  in  all  copies
 * and that both the copyright notice and this permission notice appear in
 * supporting docu- mentation,  and  that  the name of Evans & Sutherland
 * not be used in advertising or publicity pertaining to  distribution of
 * the software without specific, written prior permission.
 *
 * Written by:
 *
 * Robert C. Pendleton
 *
 * Evans & Sutherland, Interactive Systems Division, Salt Lake City, Utah.
 *
 *
 * $Header: ile.c,v 3.1 90/04/25 16:22:41 gary Locked $
 */




/*------------------------------------------------------------------*/
#define BUFFER_SIZE 70

#define HISTORY_SIZE 101

#define EOL (-2)
/*------------------------------------------------------------------*/

/* special characters used by ile */

#define del '\177'

#define CA '\1'
#define CB '\2'
#define CC '\3'
#define CD '\4'
#define CE '\5'
#define CF '\6'
#define bel '\7'
#define bs '\10'
#define CI '\11'
#define nl '\12'
#define CK '\13'
#define CL '\14'
#define cr '\15'
#define CN '\16'
#define CO '\17'
#define CP '\20'
#define CQ '\21'
#define CR '\22'
#define CS '\23'
#define CT '\24'
#define CU '\25'
#define CV '\26'
#define CW '\27'
#define CX '\30'
#define CY '\31'
#define CZ '\32'

#define esc '\33'

/*------------------------------------------------------------------*/
/* control sequences ile needs */
char   *cle = "\b";		       /* Move left */
char   *cbl = "\007";		       /* Bell */
char   *cnl = "\n";		       /* Newline */
char   *ccr = "\015";		       /* Carriage return */

/*------------------------------------------------------------------*/
/*
 * The editing routines are called through the edit variable. This allows
 * the quote and escape commands to be implemented as a straight forward
 * state machine instead of requiring state flags and complex switch
 * statements.
 */
/*------------------------------------------------------------------*/

/* line edit buffer */

static char line[BUFFER_SIZE] = {0};

static int point;		       /* insertion point */
static int length;		       /* total chars in buffer */



/* procedures to edit next character */
void    echo(CHAR);
void    echoline(STR X INT);
void    cleartoend(INT);
void    clearline(CHAR);
void    backspace(CHAR);
void    quote_edit(CHAR);
void    edit_0(CHAR);
void    edit_1(CHAR);
void    edit_2(CHAR);
void    edit_3(CHAR);
void    bell(CHAR);
void    insert(CHAR);
void    dispatch(INT X CHAR);

void    (*edit) (CHAR);

struct {			       /* history buffer */
    int     length;
    char   *line;
}       hist[HISTORY_SIZE];

int     head;			       /* insertion point */
int     here;			       /* current displayed line */

#define CHAR_SET_SIZE 256

/*------------------------------------------------------------------*/
/*
 * The action_table is used to bind sequences of keys to operations or
 * strings.
 */
/*------------------------------------------------------------------*/

typedef enum {
    is_action, is_string
}       action_type;

struct {
    action_type flag;
    union {
	void    (*action) ();
	char   *string;
    }       aors;
}       action_table[4][CHAR_SET_SIZE];

/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/*
 * The following routines are action routines that are executed by the
 * editor to carry out commands. Each routine has a single character
 * argument. Each routine is invoked with the character that caused it to
 * be invoked as its argument.
 *
 * The argument isn't always useful, but it is included to provide a
 * consistent interface for the routines.
 */
/*------------------------------------------------------------------*/
/*
 * Ring the bell on the terminal.
 */
void    bell(ch)
    char    ch;
{
    fputs(cbl, stdout);
}

/*------------------------------------------------------------------*/
/*
 * Insert a character at point in the line buffer. While we are at it
 * update the display to show the insertion.
 */
void    insert(ch)
    char    ch;
{
    int     i;

    if (ch == '\t') {
	do {
	    insert(' ');
	} while (point % 8 != 0);
	return;
    }
    if (length < (BUFFER_SIZE - 2)) {

	/* display the character */
	echo(ch);

	/* redisplay the rest of the line */

	echoline(&line[point], (length - point));

	/* move the characters in the line buffer */
	/* and put the cursor back at point */

	for (i = length; i > point; i--) {
	    line[i] = line[i - 1];
	    backspace(line[i]);
	}

	/* add the character to the line buffer */
	/* and increment point and length */

	line[point] = ch;
	length++;
	point++;
    } else {
	bell('\0');
    }
}

/*------------------------------------------------------------------*/
/*
 * Delete a character at point in the line buffer. While we are at it
 * update the display to reflect the deletion.
 */
void    delete_char_under(ch)
    char    ch;
{
    int     i;

    if (point < length) {

	/* clear to the end of the line */

	cleartoend(point);

	/* retype the rest of the line */

	echoline(&line[point + 1], (length - point - 1));

	/* build the new line */

	for (i = point + 1; i < length; i++) {
	    line[i - 1] = line[i];
	    backspace(line[i]);
	}

	length--;

	if (point > length) {
	    point = length;
	}
    }
}

/*------------------------------------------------------------------*/
/*
 * Delete the character to the left of point in the line buffer. While we
 * are at it update the display to reflect the deletion.
 */
void    delete_char(ch)
    char    ch;
{
    int     i;

    if (point > 0) {
	/* move the cursor left one character */

	backspace(line[point - 1]);

	/* clear to the end of the line */

	cleartoend(point - 1);

	/* retype the rest of the line */

	echoline(&line[point], (length - point));

	/* build the new line */

	for (i = point; i < length; i++) {
	    line[i - 1] = line[i];
	    backspace(line[i]);
	}

	length--;
	point--;
    }
}

/*------------------------------------------------------------------*/
/*
 * Bind the edit vector to quote_edit so that the next character will be
 * placed in the line buffer.
 */
void    quote(ch)
    char    ch;
{
    edit = quote_edit;
}

/*------------------------------------------------------------------*/
/*
 * The next character will select an action from action_table[1]
 */
void    escape_1(ch)
    char    ch;
{
    edit = edit_1;
}

/* The next character will select an action from action_table[2] */
void    escape_2(ch)
    char    ch;
{
    edit = edit_2;
}

/* The next character will select an action from action_table[3] */
void    escape_3(ch)
    char    ch;
{
    edit = edit_3;
}

/*------------------------------------------------------------------*/
/*
 * Move the cursor to the start of the line.
 */
void    start_of_line(ch)
    char    ch;
{
    int     i;

    if (length > 0) {
	for (i = 0; i < point; i++) {
	    backspace(line[i]);
	}
	point = 0;
    }
}

/*------------------------------------------------------------------*/
/*
 * Move the cursor one character to the left.
 */
void    backward_char(ch)
    char    ch;
{
    if ((length > 0) && (point > 0)) {
	backspace(line[point - 1]);
	point--;
    }
}

/*------------------------------------------------------------------*/
/*
 * Move the cursor to the right of the last character on the line.
 */
void    end_of_line(ch)
    char    ch;
{
    if ((length > 0) && (point < length)) {
	echoline(&line[point], (length - point));
	point = length;
    }
}

/*------------------------------------------------------------------*/
/*
 * Move the cursor one character to the right.
 */
void    forward_char(ch)
    char    ch;
{
    if ((length > 0) && (point < length)) {
	echo(line[point]);
	point++;
    }
}

/*------------------------------------------------------------------*/
/*
 * Add a line to the history buffer and pass it to the child process as
 * input.
 */
void    add_to_history(ch)
    char    ch;
{

    /*
     * Put the line in the history buffer. Make here point to the
     * current line. And increment head to point to the next history
     * slot.
     */

    /*
     * If the current line is identical to the current history line,
     * don't add it.
     */

    /* don't save blank lines */

    int     prev;

    if ((head - 1) < 0) {
	prev = HISTORY_SIZE - 1;
    } else {
	prev = head - 1;
    }

    if ((length != 0) &&
	((length != hist[prev].length) ||
	 (strncmp(hist[prev].line, line, length) != 0))) {
	/* set the length of the entry */

	hist[head].length = length;

	/* make sure there is enough storage for the new line */

	if (hist[head].line == NULL) {
	    if ((hist[head].line = (char *) malloc(length)) == NULL) {
		perror("ile");
	    }
	} else {
	    if ((hist[head].line =
		 (char *) realloc(hist[head].line, length))
		== NULL) {
		perror("ile");
	    }
	}

	(void) strncpy(hist[head].line, line, length);

	head = (head + 1) % HISTORY_SIZE;

	if (hist[head].line != NULL) {
	    free(hist[head].line);
	    hist[head].length = 0;
	    hist[head].line = NULL;
	}
    }
    /* reset here */

    here = head;

/*
 * Echo a carriage return or a newline as a cr-nl sequence. Then send the
 * line to the child process. Finally, clear the buffer for later use.
 */

    fputs(ccr, stdout);
    fputs(cnl, stdout);

    line[length] = nl;
    length++;
    line[length] = EOS;

    point = 0;
    length = 0;

}

/*------------------------------------------------------------------*/
/*
 * Erase the entire line.
 */
void    erase_line(ch)
    char    ch;
{
    /* remove any text from the display */

    clearline(ch);

    /* nothing in the line buffer */

    point = 0;
    length = 0;

    /* reset here */

    here = head;

}

/*------------------------------------------------------------------*/
/*
 * Erase from the current cursor position to the end of the line.
 */
void    erase_to_end_of_line(ch)
    char    ch;
{
    if ((length > 0) && (point < length)) {
	cleartoend(point);
	length = point;
    }
}

/*------------------------------------------------------------------*/
/*
 * Retype the current contents of the edit buffer.
 */
void    retype_line(ch)
    char    ch;
{
    int     i;

    fputs(ccr, stdout);
    fputs(cnl, stdout);

    echoline(line, length);

    for (i = point; i < length; i++) {
	backspace(line[i]);
    }
}

/*------------------------------------------------------------------*/
/*
 * Go to the the next entry in the history buffer and display it. If we
 * are past the last history entry, then beep.
 */
void    forward_history(ch)
    char    ch;
{
    if (here != head) {
	clearline(ch);

	here = (here + 1) % HISTORY_SIZE;
	length = hist[here].length;
	point = length;

	strncpy(line, hist[here].line, length);
	echoline(line, length);
    } else {
	bell('\0');
    }
}

/*------------------------------------------------------------------*/
/*
 * Go back one entry in the history buffer and display it. If we are
 * already at the last entry, then beep.
 */
void    backward_history(ch)
    char    ch;
{
    int     prev;

    prev = here - 1;

    if (prev < 0) {
	prev = HISTORY_SIZE - 1;
    }
    if (hist[prev].line != NULL) {
	clearline(ch);

	here = prev;
	length = hist[here].length;
	point = length;

	strncpy(line, hist[here].line, length);
	echoline(line, length);
    } else {
	bell('\0');
    }
}

/*------------------------------------------------------------------*/
/*
 * The following routines are utility routines used by the editing
 * routines.
 */
/*------------------------------------------------------------------*/
/*
 * Clear to the end of the current input line. Assumes that
 * line[p..length-1] corresponds to the chars at the cursor and right.
 */
void    cleartoend(p)
    int     p;
{
    int     i;

    for (i = p; i < length; i++) {
	fputc(' ', stdout);
	if (line[i] < ' ')
	    fputc(' ', stdout);
    }
    for (i = length - 1; i >= p; i--) {
	backspace(line[i]);
    }
}

/*------------------------------------------------------------------*/
/*
 * Clear the input line. Backspace to the start of the line. Then clear to
 * the end of the line.
 */
void    clearline(ch)
    char    ch;
{
    int     i;

    for (i = 0; i < point; i++) {
	backspace(line[i]);
    }

    cleartoend(0);
}

/*------------------------------------------------------------------*/
/*
 * Echo a character. Not all characters are created equal. Control
 * characters are echoed in ^X form. So they take up two character
 * positions instead of the normal 1 character position.
 */
void    echo(ch)
    char    ch;
{
    /* how should we echo the char? */

    if (ch < ' ') {
	fputc('^', stdout);
	fputc('@' + ch, stdout);
    } else {
	fputc(ch, stdout);
    }
}

/*------------------------------------------------------------------*/
/*
 * Echo a line. Print a whole line with control characters printed in ^X
 * form.
 */
void    echoline(line, length)
    char   *line;
    int     length;
{
    int     i;

    for (i = 0; i < length; i++) {
	echo(*line++);
    }

}

/*------------------------------------------------------------------*/
/*
 * Backspace over a character. Generate enough bs characters to backspace
 * over any character.
 */
void    backspace(ch)
    char    ch;
{
    if (ch < ' ') {
	fputs(cle, stdout);
	fputs(cle, stdout);
    } else {
	fputs(cle, stdout);
    }
}

/*------------------------------------------------------------------*/
/*
 * Add any character to the line buffer.
 */
void    quote_edit(ch)
    char    ch;
{
    insert(ch);

    edit = edit_0;
}

/*------------------------------------------------------------------*/
/*
 * Given a character and an action table number either execute the action
 * or pass the string to (*edit)(ch)
 */
void    dispatch(table, ch)
    int     table;
    char    ch;
{
    char   *cptr;

    switch (action_table[table][ch].flag) {
      case is_action:

	(*(action_table[table][ch].aors.action)) (ch);

	break;

      case is_string:

	cptr = action_table[table][ch].aors.string;
	while ((*cptr) != '\0') {
	    (*edit) (*cptr);
	    cptr++;
	}

	break;
    }
}

/*------------------------------------------------------------------*/
/*
 * Select an action from action_table[3] and execute it.
 */
void    edit_3(ch)
    char    ch;
{
/*
 * reset so that next input is handled by edit_0 unless over ridden by the
 * action.
 */

    edit = edit_0;
    dispatch(3, ch);
    fflush(stdout);
}

/*------------------------------------------------------------------*/
/*
 * Select an action from action_table[2] and execute it.
 */
void    edit_2(ch)
    char    ch;
{
/*
 * reset so that next input is handled by edit_0 unless over ridden by the
 * action.
 */

    edit = edit_0;
    dispatch(2, ch);
    fflush(stdout);
}

/*------------------------------------------------------------------*/
/*
 * Select an action from action_table[1] and execute it.
 */
void    edit_1(ch)
    char    ch;
{
/*
 * reset so that next input is handled by edit_0 unless over ridden by the
 * action.
 */

    edit = edit_0;
    dispatch(1, ch);
    fflush(stdout);
}

/*------------------------------------------------------------------*/
/*
 * Select an action from action_table[0] and execute it.
 */
void    edit_0(ch)
    char    ch;
{
    dispatch(0, ch);
    fflush(stdout);
}

/*------------------------------------------------------------------*/
/*
 * Input line editor.
 *
 * Initialize the world. Then loop forever using select to wait for
 * characters to be available from either stdin or from master_pty. When
 * characters are available, pass them on after doing any needed editing.
 */
void    ile(NONE);


void    ile()
{
    /* general purpose integer variable */

    int     i;

    /* set initial edit function */

    edit = edit_0;

    /* initialize line buffer */

    point = 0;
    length = 0;

    /* initialize history buffer */

    head = 0;
    here = 0;

    for (i = 0; i < HISTORY_SIZE; i++) {
	hist[i].length = 0;
	hist[i].line = NULL;
    }

}


#ifdef  VMS
static unsigned int kb = 0;
short   key_read;
Bool    stored = false;

/* uses virtual keyboard to read individual characters */
#endif


int     ile_getc()
{
    int     cc;

    /* if line[point..] is empty, re-load */
    while (line[point] == EOS) {
	/* make line empty */
	point = 0;
	line[point] = EOS;

	for (;;) {

#ifdef Raw
#ifdef Mac
	    cc = getch();
#else
	    cc = getchar();
#endif
#else

#ifdef VMS
	    if (kb == 0)
		smg$create_virtual_keyboard(&kb);
	    if( stored ) {
		cc = key_read;
		stored = false;
	    } else {
		smg$read_keystroke(&kb, &key_read);
		if( key_read >= 0400 ) {
		    cc = key_read>>8;
		    key_read &= 0377;
		    stored   = true;
		} else {
		    cc = key_read;
		}
	    }
#else
	    cc = getch();
#endif

#endif

	    if (cc == EOF) {	       /* EOF from a file, not stdin */
		(*edit) (CD);
		(*edit) (nl);
		break;
	    }
	    (*edit) (cc);
	    if (cc == EOF || cc == nl || cc == cr)
		break;
	}
    }
    cc = line[point++];
    return (cc == CZ || cc == CD) ? -1 : cc;
}


void    sig_int(x)
    char    x;
{

#ifdef Signals
    on_int(0);
#endif
}

/*------------------------------------------------------------------*/
/*
 * Set up default key bindings.
 */
void    default_bindings(NONE);
void    default_bindings()
{
    int     i;

    /* clear the action table */

    for (i = 0; i < CHAR_SET_SIZE; i++) {
	action_table[0][i].aors.action = insert;
	action_table[1][i].aors.action = bell;
	action_table[2][i].aors.action = bell;
	action_table[3][i].aors.action = bell;

	action_table[0][i].flag = is_action;
	action_table[1][i].flag = is_action;
	action_table[2][i].flag = is_action;
	action_table[3][i].flag = is_action;
    }

    /* default action_table[0] */

    action_table[0][0].aors.action = escape_3;
#ifdef VMS
    action_table[0][del].aors.action = delete_char;
    action_table[0][CA].aors.action = escape_3;
#else
    action_table[0][del].aors.action = delete_char_under;
    action_table[0][CA].aors.action = start_of_line;
#endif
    action_table[0][CB].aors.action = backward_char;
    action_table[0][CE].aors.action = end_of_line;
    action_table[0][CF].aors.action = forward_char;
    action_table[0][CK].aors.action = erase_to_end_of_line;
    action_table[0][CU].aors.action = erase_line;
    action_table[0][CL].aors.action = retype_line;
    action_table[0][CN].aors.action = forward_history;
    action_table[0][CP].aors.action = backward_history;
    action_table[0][CV].aors.action = quote;
#ifdef VMS
    action_table[0][bs].aors.action = start_of_line;
#else
    action_table[0][bs].aors.action = delete_char;
#endif
    action_table[0][esc].aors.action = escape_1;
    action_table[0][cr].aors.action = add_to_history;
    action_table[0][nl].aors.action = add_to_history;
    action_table[0][CX].aors.action = delete_char_under;

    action_table[0][CC].aors.action = sig_int;
    action_table[0][CZ].aors.action = insert;
    action_table[0][CD].aors.action = insert;

#ifdef Mac
    action_table[0][034].aors.action = backward_char;
    action_table[0][035].aors.action = forward_char;
    action_table[0][036].aors.action = backward_history;
    action_table[0][037].aors.action = forward_history;
#endif

    /* default action_table[1] ^[ c */

    action_table[1]['['].aors.action = escape_2;

    /* default action_table[2] ^[ [ */

    action_table[2]['A'].aors.action = backward_history;
    action_table[2]['B'].aors.action = forward_history;
    action_table[2]['C'].aors.action = forward_char;
    action_table[2]['D'].aors.action = backward_char;

#ifdef VMS
    /* For VMS default action for SMG chars */
    action_table[3][CR].aors.action = backward_history;
    action_table[3][CS].aors.action = forward_history;
    action_table[3][CT].aors.action = backward_char;
    action_table[3][CU].aors.action = forward_char;
#else
    /* IBM keypad */
    action_table[3][0110].aors.action = backward_history;
    action_table[3][0120].aors.action = forward_history;
    action_table[3][0115].aors.action = forward_char;
    action_table[3][0113].aors.action = backward_char;
    action_table[3][0123].aors.action = delete_char_under;
    action_table[3][0107].aors.action = start_of_line;
    action_table[3][0117].aors.action = end_of_line;
#endif


}

/*------------------------------------------------------------------*/
/*
 * Return a character or EOF. This routine reads characters from input and
 * converts them into a character using the following rules.
 *
 * The character may be a single character, a control character indicated by
 * ^x, an octal number starting with \, or an escaped character indictated
 * by \x.
 */
int     scan_char(C_FILE);
int     scan_char(input)
    FILE   *input;
{
    char    ch;
    int     value;

    ch = fgetc(input);
    switch (ch) {
      case '^':

	/* it is a control character */

	for (ch = fgetc(input); '@' <= ch; ch = ch - '@');

	break;

      case '\\':

	/* octal or an escaped character? */

	ch = fgetc(input);
	if (('0' <= ch) && (ch <= '7')) {

	    /* its an octal number */

	    value = 0;
	    while (('0' <= ch) && (ch <= '7')) {
		value = (value * 8) + (ch - '0');
		ch = fgetc(input);
	    }
	    ungetc(ch, input);

	    ch = value & 0177;	       /* make sure it is in range */
	} else {
	    /* its an escaped character */

	    ch = fgetc(input);
	}

	break;

      case '\n':

	/* the real end of the line */

	ch = EOL;

	break;

      default:

	/* it is just itself */

	break;
    }

    return (ch);

}

/*------------------------------------------------------------------*/
/*
 * Set key bindings from the users file.
 */
void    user_bindings(C_FILE);
void    user_bindings(file)
    FILE   *file;
{

#define NAME_SIZE 40

    static struct action_name_table {
	char   *name;
	void    (*action) ();
    }       action_name_table[] =
    {
	{
	    "bell", bell
	},
	{
	    "insert", insert
	},
	{
	    "delete_char", delete_char
	},
	{
	    "delete_char_under", delete_char_under
	},
	{
	    "quote", quote
	},
	{
	    "escape_1", escape_1
	},
	{
	    "escape_2", escape_2
	},
	{
	    "escape_3", escape_3
	},
	{
	    "start_of_line", start_of_line
	},
	{
	    "backward_char", backward_char
	},
	{
	    "end_of_line", end_of_line
	},
	{
	    "forward_char", forward_char
	},
	{
	    "add_to_history", add_to_history
	},
	{
	    "erase_line", erase_line
	},
	{
	    "erase_to_end_of_line", erase_to_end_of_line
	},
	{
	    "retype_line", retype_line
	},
	{
	    "forward_history", forward_history
	},
	{
	    "backward_history", backward_history
	},
	{
	    "", NULL
	}
    };

    char    name[NAME_SIZE];

    char    ch;
    int     i;

    int     line = 0;
    int     table;
    int     entry;


    /* Now read the character binding pairs */

    while ((ch = fgetc(file)) != EOF) {
	switch (ch) {
	  case '\n':

	    /* skipping a blank line */
	    line++;

	    break;

	  case '0':
	  case '1':
	  case '2':
	  case '3':

	    /* which table is this entry directed to? */

	    table = ch - '0';

	    /* get the character code */

	    entry = scan_char(file);

	    /* make sure the '=' is there */

	    ch = fgetc(file);
	    if (ch != '=') {
		fprintf(stderr, "ile: \'=' missing on line %d\n", line);
		exit(1);
	    }
	    /* collect the action name or string */

	    for (ch = scan_char(file), i = 0;
		 (ch != EOL) && (i < (NAME_SIZE - 1));
		 ch = scan_char(file), i++) {
		name[i] = ch;
		name[i + 1] = '\0';
	    }

	    /* look it up in the action_name_table */

	    for (i = 0;
		 (action_name_table[i].action != NULL) &&
		 (strcmp(name, action_name_table[i].name) != 0);
		 i++);

	    /* if it was found, put it in the action array */

	    if (action_name_table[i].action == NULL) {
		/* must be a string */

		action_table[table][entry].flag = is_string;
		action_table[table][entry].aors.string =
		    (char *) malloc(strlen(name) + 1);
		strcpy(action_table[table][entry].aors.string, name);
	    } else {
		/* its an action */

		action_table[table][entry].flag = is_action;
		action_table[table][entry].aors.action =
		    action_name_table[i].action;
	    }

	    line++;		       /* count the line */

	    break;

	  default:
	    fprintf(stderr,
		    "\nile: error in initialization file on line %d\n",
		    line);
	    exit(1);
	    break;
	}
    }

    fclose(file);
}

/*------------------------------------------------------------------*/
/*
 * Initialize key bindings.
 */
FILE   *open_init(STR);

void    initialize(STR ARRAY);
void    initialize(argv)
    char   *argv[];

{
    FILE   *file;

    /* set up the default bindings */

    default_bindings();

    /* Look for an initialization file. If it's there, load it. */

    if ((file = open_init("ile")) != NULL) {
	user_bindings(file);
    }
}
#endif

