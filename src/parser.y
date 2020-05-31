%{
# include	"common.h"
# include	"ast.h"
# include	"scanner.h"
# include       "micro.h"
AST_Ptr make_func();
void fix_error();

# define	Not_Implemented \
		mcprintf("Unimplemented feature\n",0,0)
Bool    need_clear = false;

#define YYSTYPE AST_Ptr
%}
%token	T_Id T_Integer T_String T_Real T_Boolean CLEAR
%token  T_Source


%right		T_Gets
%nonassoc	T_Where
%right		T_St
%nonassoc	T_Iff
%left		T_Impl
%left		T_Or
%left		T_And
%right		T_Not
%nonassoc	T_In T_Notin T_Subset T_Lt T_Le T_Eq T_Ne T_Gt T_Ge
%left		INFIX '.'
%left		T_Plus T_Minus T_With T_Less
%left		T_Times T_Slash T_Mod T_Div
%right		T_Exp
%nonassoc	T_Insert
%nonassoc	'?'
%nonassoc	'@'
%right		UNARY
%left		CALL '(' '{'

%token  CALL
%token  UNARY
%token	T_Dotdot T_Missing T_Arrow
%token	T_Take	T_To
%token  T_Choose
%token	T_Do	T_Else	T_Elseif
%token	T_End	T_Exists	T_False T_For	T_Forall
%token	T_From	T_Fromb T_Frome T_Func	T_If	T_Local T_Less
%token	T_Newat T_Not	T_Of	T_Om	T_Opt	T_Print T_PrintF
%token 	T_Proc	T_Prog	T_Read	T_ReadF T_Return
%token	T_Then	T_True	T_Value T_Where T_While T_With
%token  T_Write T_Writeln

%token	MAP SELECTOR ITERS DEREF

%start program

%%
id:	T_Id	      {$$ = New_AST(T_Id, (AST_Ptr)str((char*)$1), ANULL);}
    ;

program:
	simple.program		{parsed = $1; return  0;}
    |	stmt ';'		{parsed = $1; return  0;}
    |	error
        { fix_error(); }
	CLEAR		{ need_clear = false; }

	program		{ $$ = $5; }
    ;


end.prog:
	T_End
    |	T_End T_Prog
    ;

simple.program:
	T_Prog id ';' dcls body end.prog ';'
	{$$ = New_AST(T_Prog, $4, $5);}
    ;

body:
	stmts
    ;

stmts:  
        stmt.list opt.semi      {$$ = Rev_Seq($1);}
    ;

opt.semi: /* empty */ | ';'  ;

stmt.list:
	stmt.list ';' stmt	{$$ = New_AST(0, $3, $1);}
    |	stmt	 		{$$ = New_AST(0, $1, ANULL);}
    ;

stmt:   { $$ = New_AST(T_Source, Source_List,ANULL); Source_List = $$; } 
        real.stmt
        { $$ = $1; ast_2($1)= $2;}
    ;
/* Link Source nodes through Source_List */
/* When each line is completed, store line as String in Source nodes */


opt.from: {$$ = ANULL;}
    |   T_From expr   { $$=$2; }
    ;

opt.to: {$$ = ANULL;}
    |   T_To expr   { $$=$2; }
    ;



real.stmt:
	T_Return	{$$ = New_AST(T_Return,ANULL,ANULL);}
    |	T_Return  expr	{$$ = New_AST(T_Return,$2,ANULL);}
    |	T_Read    expr.list opt.from	{$$ = New_AST(T_Read, $2, $3);}
    |	T_ReadF   pair.list opt.from	{$$ = New_AST(T_ReadF, $2, $3);}
    |	T_Print   expr.list opt.to	{$$ = New_AST(T_Print, $2, $3);}
    |	T_PrintF  pair.list opt.to	{$$ = New_AST(T_PrintF, $2, $3);}
    |	T_Write   pair.list opt.to	{$$ = New_AST(T_PrintF, $2, $3);}
    |	T_Writeln pair.list opt.to
            {$$ = New_AST(T_PrintF, 
			  New_AST(0,
				  New_AST(T_Om,ANULL,ANULL),
				  New_AST(0, New_AST(T_String,
						     (AST_Ptr)str("\n"),ANULL),
					  $2)),
			  $3);}
    |	T_Writeln  opt.to
            {$$ = New_AST(T_PrintF, 
			  New_AST(0,
				  New_AST(T_Om,ANULL,ANULL),
				  New_AST(0, New_AST(T_String,
						     (AST_Ptr)str("\n"),ANULL),
					  ANULL)),
			  $2);}
    |	assignment
    |	expr		{$$ = New_AST(CALL, $1, ANULL); }
    |	loop.stmt
    |	from.stmt
    ;


else.part:
	T_Else stmts		{$$ = $2;}
    |	/* empty */		{$$ = ANULL; }
    ;

end.if:
	T_End
    |	T_End T_If
    ;

if.stmt:
	T_If expr T_Then stmts else.ifs else.part end.if
	{$$ = New_AST(T_If, New_AST(0, $2, $4),
			    New_AST(0, Rev_Seq($5), $6) ); }
    ;
	
else.ifs:
	else.ifs T_Elseif expr T_Then stmts
	{$$ = New_AST(T_Elseif, New_AST(0, $3, $5),
				$1); }

    |	/* empty */			{$$ = ANULL; }
    ;

end.for:
	T_End
    |	T_End T_For
    ;

end.while:
	T_End
    |	T_End T_While
    ;

loop.stmt:
	T_For iterator	T_Do stmts end.for
	{$$ = New_AST(T_For, $2, $4);}
    |	T_While expr	T_Do stmts end.while
	{$$ = New_AST(T_While, $2, $4);}
    ;

iterator:
	iter.list
	{$$ = New_AST(ITERS, $1, ANULL);}
    |	iter.list  T_St expr
	{$$ = New_AST(ITERS, $1, $3);}
    ;

iter.list:
	simple.iterator ',' iter.list
	{$$ = Spread($1, $3);}
    |	simple.iterator
	{$$ = Spread($1, ANULL);}
    ;

simple.iterator:
	bound.list  T_In expr	
	{$$ = New_AST(T_In, $1, $3);}
    |	bound T_Eq expr
	{$$ = New_AST(MAP, $1, $3);}
    ;


selector:
	'{' expr.list '}'
	{$$ = New_AST('{', $2, ANULL);}
    |	'(' expr.list ')'
	{$$ = New_AST('(', $2, ANULL);}

    |	'(' expr T_Dotdot      ')'
	{$$ = New_AST(T_Dotdot, $2, ANULL);}
    |	'('	 T_Dotdot expr ')'
	{$$ = New_AST(T_Dotdot, ANULL, $3);}
    |	'(' expr T_Dotdot expr ')'
	{$$ = New_AST(T_Dotdot, $2, $4);}
    |	'(' ')'
	{$$ = New_AST(CALL, ANULL, ANULL);}
    ;

assignment:
	expr	   T_Gets expr
	{$$ = New_AST(T_Gets, $1, $3);}
    ;

expr:

	id
    |	if.stmt
    |	T_Missing
	{$$ = New_AST(T_Missing, ANULL, ANULL);}

    |	T_Integer     {$$ = New_AST(T_Integer, (AST_Ptr)str((char*)$1),
					   ANULL);}
    |	T_Real	      {$$ = New_AST(T_Real,    (AST_Ptr)str((char*)$1),
					   ANULL);}
    |	T_String      {$$ = New_AST(T_String,  (AST_Ptr)str((char*)$1),
					   ANULL);}

    |	T_True	      {$$ = New_AST(T_True,  ANULL, ANULL);}
    |	T_False	      {$$ = New_AST(T_False, ANULL, ANULL);}
    |	T_Om	      {$$ = New_AST(T_Om,    ANULL, ANULL);}
    |	T_Newat	      {$$ = New_AST(T_Newat, ANULL, ANULL);}

    |	func.const

    |	where.clause

    |	'(' expr ')'		{$$ = $2;}

    |	'[' former ']'		{$$ = New_AST('[', $2, ANULL); }
    |	'{' former '}'		{$$ = New_AST('{', $2, ANULL); }
    |	'@' expr                %prec UNARY
				{$$ = New_AST(DEREF, $2, ANULL); }
    |	'@' '(' expr ',' expr.list ')'	%prec UNARY
				{$$ = New_AST(DEREF, $3, $5); }
    |	'#' expr	%prec UNARY
				{$$ = New_AST('#', $2, ANULL); }

    |	expr selector	%prec CALL
				{$$ = New_AST(SELECTOR, $1, $2);}

    |	expr '.' id	expr	%prec INFIX
	{$$ = New_AST(SELECTOR, $3,
		    New_AST( '(', New_AST(0, $4,
					 New_AST(0, $1, ANULL)),
				  ANULL) );}

    |	expr '.' '(' expr ')'	expr	%prec INFIX
	{$$ = New_AST(SELECTOR, $4,
		    New_AST( '(', New_AST(0, $6,
					 New_AST(0, $1, ANULL)),
				  ANULL) );}

    |	expr '?'	expr  {$$ = New_AST('?',      $1, $3);}
    |   expr '@'        expr  {$$ = New_AST('@',      $1, $3);}
    |	expr T_Exp	expr  {$$ = New_AST(T_Exp,    $1, $3);}
    |	expr T_Times	expr  {$$ = New_AST(T_Times,  $1, $3);}
    |	expr T_Slash	expr  {$$ = New_AST(T_Slash,  $1, $3);}
    |	expr T_Mod	expr  {$$ = New_AST(T_Mod,    $1, $3);}
    |	expr T_Div	expr  {$$ = New_AST(T_Div,    $1, $3);}
    |	expr T_Plus	expr  {$$ = New_AST(T_Plus,   $1, $3);}
    |	expr T_Minus	expr  {$$ = New_AST(T_Minus,  $1, $3);}
    |	expr T_With	expr  {$$ = New_AST(T_With,   $1, $3);}
    |	expr T_Less	expr  {$$ = New_AST(T_Less,   $1, $3);}
    |	expr T_Eq	expr  {$$ = New_AST(T_Eq,     $1, $3);}
    |	expr T_Ne	expr  {$$ = New_AST(T_Ne,     $1, $3);}
    |	expr T_Lt	expr  {$$ = New_AST(T_Lt,     $1, $3);}
    |	expr T_Gt	expr  {$$ = New_AST(T_Gt,     $1, $3);}
    |	expr T_Le	expr  {$$ = New_AST(T_Le,     $1, $3);}
    |	expr T_Ge	expr  {$$ = New_AST(T_Ge,     $1, $3);}
    |	expr T_In	expr  {$$ = New_AST(T_In,     $1, $3);}
    |	expr T_Notin	expr  {$$ = New_AST(T_Notin,  $1, $3);}
    |	expr T_Subset	expr  {$$ = New_AST(T_Subset, $1, $3);}
    |	expr T_And	expr  {$$ = New_AST(T_And,    $1, $3);}
    |	expr T_Or	expr  {$$ = New_AST(T_Or,     $1, $3);}
    |	expr T_Impl	expr  {$$ = New_AST(T_Impl,   $1, $3);}
    |	expr T_Iff	expr  {$$ = New_AST(T_Iff,    $1, $3);}

    |	expr T_Insert binop expr	
	{$$ = New_AST(T_Insert, $3, New_AST(0, $1, $4) );}
			
    |	T_Insert binop expr	
	{$$ = New_AST(T_Insert, $2, 
		      New_AST(0, ANULL, $3) );}
	
    |	T_Not expr
	{$$ = New_AST(T_Not, ANULL, $2);}

    |	T_Plus expr		%prec UNARY		
	{$$ = New_AST(T_Plus, ANULL, $2);}

    |	T_Minus expr		%prec UNARY		
	{$$ = New_AST(T_Minus, ANULL, $2);}

    |	T_Exists iter.list T_St expr		
	{$$ = New_AST(T_Exists, $2, $4);}

    |	T_Choose iter.list T_St  expr		
	{$$ = New_AST(T_Choose, $2, $4);}

    |	T_Forall iter.list T_St  expr		
	{$$ = New_AST(T_Forall, $2, $4);}
    ;


expr.list:	/* builds list in reverse order */
	expr			
	{$$ = New_AST(0, $1, ANULL); }

    |	expr.list ',' expr
	{$$ = New_AST(0, $3, $1); }
    ;


pair.list:	/* builds in reverse order */
	expr		{$$ = New_AST(0, New_AST(T_Om,ANULL,ANULL),
					 New_AST(0,$1, ANULL)); }
    |	expr T_St expr	{$$ = New_AST(0,$3,New_AST(0,$1,ANULL)); }

    |	pair.list ',' expr	
	{$$ = New_AST(0, New_AST(T_Om,ANULL,ANULL),
			 New_AST(0, $3, $1)); }
    |	pair.list ',' expr T_St expr	{$$ = New_AST(0,$5,New_AST(0,$3,$1)); }
    ;

counted:
	expr.list T_Dotdot expr
	{$$ = New_AST(T_Dotdot, $1, $3);}
    ;


from.stmt:
	T_Take expr T_Frome expr	{$$= New_AST(T_Frome, $2, $4);}
    |	T_Take expr T_Fromb expr	{$$= New_AST(T_Fromb, $2, $4);}
    |	T_Take expr T_From  expr	{$$= New_AST(T_From,  $2, $4);}
    ;

former:
	/* empty */		{$$ = ANULL;}
    |	expr T_St iterator	{$$ = New_AST(T_St, $1, $3);}
    |	expr.list
    |	counted 
    ;

binop: binop.short { $$ = New_AST((int)$$, ANULL,ANULL); }
    |	id
    |	'.' id		{$$ = $2;}
    |	'(' expr ')'	{$$ = New_AST('(',	$2,    ANULL);}
    |	'.' '(' expr ')'	{$$ = New_AST('(',	$3,    ANULL);}
    ;

binop.short:
	T_Exp		
    |	T_Times		
    |	T_Slash		
    |	T_Mod		
    |	T_Div		
    |	T_Plus		
    |	T_Minus		
    |	T_With		
    |	T_Less		
    |	T_Eq		
    |	T_Ne		
    |	T_Lt		
    |	T_Gt		
    |	T_Le		
    |	T_Ge		
    |	T_In		
    |	T_Notin		
    |	T_Subset	
    |	T_And		
    |	T_Or		
    |	T_Impl		
    ;

/* token numbers are small enough to fit in pointers */
/* the value is only used for comparison purposes,   */
/* only stored in kind fields (not pointer fields     */
t_func: T_Func {$$=(YYSTYPE)T_Func;} | T_Proc {$$=(YYSTYPE)T_Proc;}
end.func:
        T_End {$$=(YYSTYPE)T_End; /* presumes |ptr| >= |int| */}
    |	T_End t_func    {$$ = $2;}
    ;


func.const:
	func.head dcls stmts end.func
        { $$ = make_func($1,$2,$3,$4); }

    |  T_St 
        { $$ = New_AST(T_Source, Source_List,ANULL); Source_List = $$;
          if( func_level == 0 ) {
	      Func_Tail = Func_List = mcCons(Nil,Nil);
          }
	  func_level++;
        } 
       args T_Arrow 
	{ $$ = New_AST(0,$2,$3); }
       expr T_St   /* func(args); return expr; end */
            { func_level --;
	      if( func_level==0 ) Source_Flush();

	      $$ = New_AST(T_Func, 
		     New_AST(T_Source,
			     (func_level==0 ? Func_List : Nil),
			     ast_2($5)), /* ARGS */
		     New_AST(0, ANULL, /* DCLS */
			     New_AST(0, 
				     $2, /* First line */
				     New_AST(0, New_AST(T_Return, $6, ANULL),
					     ANULL))));
	      if(func_level==0) {
		  Func_List = Nil;
	      }
	    }
    ;

args:   id.list opt.part                { $$ = New_AST(0,$1,$2); }
    |   opt.part                        { $$ = New_AST(0,ANULL,$1); }
    ;

func.head:  /* Returns (source,args) */
	t_func 
        { $$ = New_AST(T_Source, Source_List,ANULL); Source_List = $$;
          if( func_level == 0 ) {
	      Func_Tail = Func_List = mcCons(Nil,Nil);
          }
	  func_level++;
        } 
        '(' args  ')' ';'
	{ $$ = New_AST((int)$1,$2,$4); /* presumes |ptr| >= |int| */ }
    ;

opt.part:
	/* empty */				{ $$ = ANULL; }
    |	T_Opt id.list				{ $$ = $2; }
    ;

dcls:
	/* empty */			{ $$ = ANULL; }
     |	T_Local id.list ';' dcls	{ $$ = New_AST(T_Local, $2, $4); }
     |	T_Value id.list ';' dcls	{ $$ = New_AST(T_Value, $2, $4); }
    ;

id.list:
	id				{ $$ = New_AST(0, $1, ANULL); }
    |	id ',' id.list			{ $$ = New_AST(0, $1, $3  ); }
    ;

end.where:
	T_End
    ;

where.clause:
	expr T_Where defns end.where	{ $$ = New_AST(T_Where, $1, $3); }
    ;

defns:
	defn			{ $$ = New_AST(0, $1, ANULL); }
    |	defn ';'		{ $$ = New_AST(0, $1, ANULL); }
    |	defn ';' defns		{ $$ = New_AST(0, $1, $3); }
    ;

defn:
	bound T_Gets expr 
	    { $$ = New_AST( T_Gets, $1,$3); }
    |	id selector T_Gets expr 
	    { $$ = New_AST(SELECTOR, New_AST(SELECTOR, $1, $2), $4); }
    ;

bound:
	T_Missing		{$$ = New_AST(T_Missing, ANULL, ANULL);}
    |	id
    |	'[' bound.list ']'	{$$ = New_AST('[', $2, ANULL); }
    ;

bound.list:
	bound			{ $$ = New_AST(0, $1, ANULL); }
    |	bound.list ',' bound	{ $$ = New_AST(0, $3, $1); }
    ;
%%

void yyerror(i) char *i; {
}

void fix_error() 
	{     mcprintf("!Syntax error or clearing input at or before end of:",0,0);
	      Show_Error();

	      if( yyin==stdin ) {
		  mcprintf("\n!Enter '!clear'" ,0,0);
		  mcprintf("\n",0,0);
		  need_clear = true;
	      } else {
		  mcputc('\n');
		  yyclear();
                  ERROR;
	      }
	}

AST_Ptr make_func(fhead,dcls,stmts,endfunc) 
    AST_Ptr fhead,dcls,stmts,endfunc;
    { AST_Ptr result; 
      /* presumes |ptr| >= |int| and comparable */
      if( endfunc != (YYSTYPE)T_End && 
	   endfunc!= (YYSTYPE)ast_kind(fhead) ) {
	SYNTAX("Mis-matched 'end'");
      }
      func_level --;
      if( func_level==0 ) Source_Flush();

      result = New_AST(ast_kind(fhead), 
		   New_AST(T_Source,
			   (func_level==0 ? Func_List : Nil),
			   ast_2(fhead)), /* ARGS */
		   New_AST(0, dcls,      /* dcls */
			   New_AST(0,ast_1(fhead), /* First line */
				   stmts)) );
      if(func_level==0) {
	  Func_List = Nil;
      }
      return result;
    }
