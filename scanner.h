extern FILE *yyin;
extern char yytext[];
extern int  line_num;
extern AST_Ptr yylval;
extern int pushes;

int     immediate_char(NONE);
void	Source_Flush(NONE);
FILE   *next_file(NONE);
FILE   *open_init(STR);
void    prompt(STR);
void    Clear(NONE);
void    Clear_Char(NONE);
int     Refresh_Char(NONE);
int     New_Char(NONE);
int     catch(NONE);
void    fpush(C_FILE X INT);
int     fpop(NONE);
int     Edit(NONE);

void    yyclear(NONE);
int     yywrap(NONE);
void    yyerror(STR);
int     yylex(NONE);

char   *str(STR);		       /* Return an new copy of string p */
AST_Ptr Spread(AST_PTR X AST_PTR);
