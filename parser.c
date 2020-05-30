
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
# define T_Id 257
# define T_Integer 258
# define T_String 259
# define T_Real 260
# define T_Boolean 261
# define CLEAR 262
# define T_Source 263
# define T_Gets 264
# define T_Where 265
# define T_St 266
# define T_Iff 267
# define T_Impl 268
# define T_Or 269
# define T_And 270
# define T_Not 271
# define T_In 272
# define T_Notin 273
# define T_Subset 274
# define T_Lt 275
# define T_Le 276
# define T_Eq 277
# define T_Ne 278
# define T_Gt 279
# define T_Ge 280
# define INFIX 281
# define T_Plus 282
# define T_Minus 283
# define T_With 284
# define T_Less 285
# define T_Times 286
# define T_Slash 287
# define T_Mod 288
# define T_Div 289
# define T_Exp 290
# define T_Insert 291
# define UNARY 292
# define CALL 293
# define T_Dotdot 294
# define T_Missing 295
# define T_Arrow 296
# define T_Take 297
# define T_To 298
# define T_Choose 299
# define T_Do 300
# define T_Else 301
# define T_Elseif 302
# define T_End 303
# define T_Exists 304
# define T_False 305
# define T_For 306
# define T_Forall 307
# define T_From 308
# define T_Fromb 309
# define T_Frome 310
# define T_Func 311
# define T_If 312
# define T_Local 313
# define T_Newat 314
# define T_Of 315
# define T_Om 316
# define T_Opt 317
# define T_Print 318
# define T_PrintF 319
# define T_Proc 320
# define T_Prog 321
# define T_Read 322
# define T_ReadF 323
# define T_Return 324
# define T_Then 325
# define T_True 326
# define T_Value 327
# define T_While 328
# define T_Write 329
# define T_Writeln 330
# define MAP 331
# define SELECTOR 332
# define ITERS 333
# define DEREF 334
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256



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
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 173,
	63, 0,
	-2, 82,
-1, 174,
	64, 0,
	-2, 83,
-1, 184,
	272, 0,
	273, 0,
	274, 0,
	275, 0,
	276, 0,
	277, 0,
	278, 0,
	279, 0,
	280, 0,
	-2, 93,
-1, 185,
	272, 0,
	273, 0,
	274, 0,
	275, 0,
	276, 0,
	277, 0,
	278, 0,
	279, 0,
	280, 0,
	-2, 94,
-1, 186,
	272, 0,
	273, 0,
	274, 0,
	275, 0,
	276, 0,
	277, 0,
	278, 0,
	279, 0,
	280, 0,
	-2, 95,
-1, 187,
	272, 0,
	273, 0,
	274, 0,
	275, 0,
	276, 0,
	277, 0,
	278, 0,
	279, 0,
	280, 0,
	-2, 96,
-1, 188,
	272, 0,
	273, 0,
	274, 0,
	275, 0,
	276, 0,
	277, 0,
	278, 0,
	279, 0,
	280, 0,
	-2, 97,
-1, 189,
	272, 0,
	273, 0,
	274, 0,
	275, 0,
	276, 0,
	277, 0,
	278, 0,
	279, 0,
	280, 0,
	-2, 98,
-1, 190,
	272, 0,
	273, 0,
	274, 0,
	275, 0,
	276, 0,
	277, 0,
	278, 0,
	279, 0,
	280, 0,
	-2, 99,
-1, 191,
	272, 0,
	273, 0,
	274, 0,
	275, 0,
	276, 0,
	277, 0,
	278, 0,
	279, 0,
	280, 0,
	-2, 100,
-1, 192,
	272, 0,
	273, 0,
	274, 0,
	275, 0,
	276, 0,
	277, 0,
	278, 0,
	279, 0,
	280, 0,
	-2, 101,
-1, 196,
	267, 0,
	-2, 105,
-1, 213,
	291, 0,
	-2, 107,
-1, 253,
	291, 0,
	-2, 106,
-1, 286,
	301, 13,
	302, 13,
	303, 13,
	-2, 16,
	};
# define YYNPROD 185
# define YYLAST 2178
short yyact[]={

  60, 326, 153,   4, 319,  54,  10,  20, 233, 242,
 337, 325, 333,  58,  55,  62, 154,  62,  62,  62,
 329, 328, 290, 309, 307, 295, 284, 255, 226, 163,
 160,  53, 160, 221, 144, 227, 100, 102, 102, 106,
 108,  99, 135, 136, 137, 225, 152,  71, 163, 149,
 150, 151, 224, 160, 217, 300, 235,   3, 257,  56,
  10, 239, 241,  70,  72,  73, 242, 160,   5, 169,
 170, 220, 140, 173, 174, 175, 176, 177, 178, 179,
 180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
 190, 191, 192, 193, 194, 195, 196,  39,  99, 198,
 204, 215,  35,   1, 158, 147,  99,  99, 212, 211,
 213,  99, 109, 172, 216, 208, 331, 101, 318, 289,
 220,  72,  73, 287,  98, 159,  38, 286, 256,  72,
  73,  73, 324, 148,  57, 160,   7,  67, 260, 288,
 218, 160, 317, 103, 243, 199, 254, 156, 259, 316,
 155,  51, 283,  36, 110, 105, 104,  59,  61,  63,
  34, 247, 248,  33, 249, 250, 139, 246,  99, 273,
 308, 306, 251, 252,  71,  64,  65,  66, 138, 145,
 146,  98, 310,  24, 332,  37, 327, 162,  22,  98,
  98,  72,  73,  21,  98,  19, 240,  11, 253,   6,
  10, 165, 166, 167, 168, 285, 262,  39, 234, 197,
 245, 264,  35, 206,   3, 141, 266, 222, 268, 294,
 270, 157, 272,   8,   2, 274, 275,   0, 277,   0,
 279, 280, 281,   0,   0, 276,  38, 278, 142,   0,
   0, 282, 202, 203,   0,   0,   0,   0,   0,   0,
   0,  98,   0,   0,   0,   0,   0,   0, 299,   0,
   0, 244, 302,  36,   0, 258,  97,   0,  95,  94,
  93,  92,   0,  89,  90,  91,  85,  87,  83,  84,
  86,  88, 210,  79,  80,  81,  82,  75,  76,  77,
  78,  74,  96, 161, 161,  37,   0, 320, 321, 219,
  99, 322,  68, 291,   0, 293,  39,  68,   0,   0,
   0,  35, 301, 200,   0, 263, 311, 330,  10,  10,
  26,  28,  27,  72,  73,   0, 338,   0,  52, 334,
  10,   0,   0,  41, 313,  38, 315,   0, 335,   0,
   0,   0,   0, 312,  42,  43,   0, 339,  74,  96,
 236, 238, 269,  40,   0,   0, 298,  25,   0,  49,
 223,  45,  36,   0,   0,   0,  44,  30,  47,  46,
  39,   0,   0,  54,  50,  35,  32,   0,  31,   0,
  15,  16,  55,  98,  13,  14,  12,   0,  29,   0,
  48,  17,  18,  97,  37,  95,  94,  93,  92,  38,
  89,  90,  91,  85,  87,  83,  84,  86,  88, 304,
  79,  80,  81,  82,  75,  76,  77,  78,  74,  96,
   0,   0,   0,   0,   0,   0,  36,   0,   0,  10,
  26,  28,  27,   0,   0,  39, 271,   0,  52, 292,
  35,   0,   0,  41,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 232,  42,  43,   0,   0,  37,   0,
   0,   0,   0,  40,  38,   0, 205,  25,   0,   0,
   0,  45, 200,   0,   0,   0,  44,  30,   0,  46,
   0,   0,   0,  54,  50, 314,  32,   0,  31,   0,
   0,  36,  55,   0,   0,   0,   0,   0,  29,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  39,   0,   0,   0,   0, 107,   0,   0,   0,
   0,   0,   0,  37,   0,   0,   0,   0,  10,  26,
  28,  27,   0,   0,   0,   0,   0,  52,   0,   0,
  38,   0,  41,   0,   0,   0,  75,  76,  77,  78,
  74,  96,   0,  42,  43,   0,   0,   0,   0,   0,
   0,   0,  40,   0,   0,   0,  25,  36,   0,   0,
  45,   0,   0,   0,   0,  44,  30,   0,  46,   0,
  99,   0,  54,  50,   0,  32,  71,  31,   0,   0,
   0,  55,  10,  26,  28,  27,   0,  29,   0,  37,
   0,  52,   0,  72,  73,   0,  41,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,  42,  43,   0,
   0,   0,   0,   0,   0,   0,  40,   0,   0,   0,
  25,   0,   0,  68,  45,   0,   0,   0,   0,  44,
  30,   0,  46,   0,  99,   0,  54,  50,   0,  32,
  71,  31,   0,   0,   0,  55,   0,  10,  26,  28,
  27,  29,   0,  98,   0,   0,  52,  72,  73,   0,
   0,  41,   0,  99,   0,   0,   0,   0,   0,  71,
   0,   0,  42,  43,   0,   0,  99,   0,   0,   0,
   0,  40,  71,   0,   0,  25,  72,  73,   0,  45,
   0,   0,   0,   0,  44,  30,   0,  46,   0,  72,
  73,  54,  50,   0,  32,   0,  31,  99, 207,   0,
  55, 265,   0,  71,   0,   0,  29,  98,   0,   0,
   0,   0,   0,  10,  26,  28,  27,   0,   0,   0,
  72,  73,  52,   0,   0,   0,   0,  41,   0,  99,
   0,   0,   0,   0,   0,  71,  98,   0,  42,  43,
   0,   0,   0,   0,   0,   0,   0,  40,   0,  98,
  99,  25,  72,  73,   0,  45,   0,   0,   0,   0,
  44,  30,   0,  46,   0,   0,   0,  54,  50,   0,
  32,   0,  31,  72,  73,   0,  55,   0,   0,   0,
  98,   0,  29,   0,   0,  97,   0,  95,  94,  93,
  92,   0,  89,  90,  91,  85,  87,  83,  84,  86,
  88,   0,  79,  80,  81,  82,  75,  76,  77,  78,
  74,  96,  98,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0, 231, 230,
 229,   0,   0,  98,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  97,
   0,  95,  94,  93,  92,   0,  89,  90,  91,  85,
  87,  83,  84,  86,  88,   0,  79,  80,  81,  82,
  75,  76,  77,  78,  74,  96,   0,   0,  97,   0,
  95,  94,  93,  92, 228,  89,  90,  91,  85,  87,
  83,  84,  86,  88,   0,  79,  80,  81,  82,  75,
  76,  77,  78,  74,  96,   0,   0, 261,  79,  80,
  81,  82,  75,  76,  77,  78,  74,  96,   0,   0,
   0,   0,  97,   0,  95,  94,  93,  92,   0,  89,
  90,  91,  85,  87,  83,  84,  86,  88,   0,  79,
  80,  81,  82,  75,  76,  77,  78,  74,  96,   0,
   0,   0,   0,   0,  97, 336,  95,  94,  93,  92,
   0,  89,  90,  91,  85,  87,  83,  84,  86,  88,
   0,  79,  80,  81,  82,  75,  76,  77,  78,  74,
  96,  99, 323,   0,   0,   0,   0,  71,   0,   0,
   0,   0,  79,  80,  81,  82,  75,  76,  77,  78,
  74,  96,   0,   0,  72,  73,   0,   0,  99, 305,
   0,   0,   0,   0,  71,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  72,  73,   0,   0,  99, 303,   0,   0,   0,
   0,  71,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  72,  73,
   0,   0,  99, 297,  98,   0,   0,   0,  71,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,  72,  73,   0,   0,  99,
   0,  98,   0,   0,   0,  71,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  72,  73,   0,   0,  99, 267,  98,   0,
   0,   0,  71,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  72,
  73,   0,   0,   0,   0,  98,   0,   0,   0,   0,
   0,  99,   0,   0,   0,   0,   0,  71,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  98,   0,  72,  73,   0,   0,  99, 207,
   0,   0,   0,   0,  71,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  98,
   0,  72,  73,   0,   0,   0,  97,   0,  95,  94,
  93,  92,   0,  89,  90,  91,  85,  87,  83,  84,
  86,  88,   0,  79,  80,  81,  82,  75,  76,  77,
  78,  74,  96,  97,  98,  95,  94,  93,  92,   0,
  89,  90,  91,  85,  87,  83,  84,  86,  88,   0,
  79,  80,  81,  82,  75,  76,  77,  78,  74,  96,
  97,  98,  95,  94,  93,  92,   0,  89,  90,  91,
  85,  87,  83,  84,  86,  88,   0,  79,  80,  81,
  82,  75,  76,  77,  78,  74,  96,  97,   0,  95,
  94,  93,  92,   0,  89,  90,  91,  85,  87,  83,
  84,  86,  88,   0,  79,  80,  81,  82,  75,  76,
  77,  78,  74,  96,  97, 296,  95,  94,  93,  92,
   0,  89,  90,  91,  85,  87,  83,  84,  86,  88,
   0,  79,  80,  81,  82,  75,  76,  77,  78,  74,
  96,  97,   0,  95,  94,  93,  92,   0,  89,  90,
  91,  85,  87,  83,  84,  86,  88,   0,  79,  80,
  81,  82,  75,  76,  77,  78,  74,  96,   0,   0,
   0,   0,   0,   0,   0,   0,  97, 209,  95,  94,
  93,  92,   0,  89,  90,  91,  85,  87,  83,  84,
  86,  88,   0,  79,  80,  81,  82,  75,  76,  77,
  78,  74,  96,  97,   0,  95,  94,  93,  92,   0,
  89,  90,  91,  85,  87,  83,  84,  86,  88,   0,
  79,  80,  81,  82,  75,  76,  77,  78,  74,  96,
  99,   0,   0,   0,   0,   0,  71,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  72,  73,   0,   0,   0,  99,   0,
   0,   0,   0,   0,  71,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  72,  73,   0,   0,  99,   0,   0,   0,   0,
   0,  71,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  72,  73,
  99,   0,   0,  98,   0,   0,  71,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  72,  73,   0,  99,   0,   0,   0,
   0,  98,  71,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  72,
  73,  99,   0,   0,   0,   0,   0,  71,  98,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,  72,  73,   0,   0,  99,   0,
   0,   0,   0,  98,  71,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  72,  73,   0,   0,   0,  99,   0,   0,  98,
   0,   0,  71,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  72,
  73,   0,   0,   0,  98,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,  97, 164,  95,  94,  93,
  92,   0,  89,  90,  91,  85,  87,  83,  84,  86,
  88,  98,  79,  80,  81,  82,  75,  76,  77,  78,
  74,  96,  69,  97,   0,  95,  94,  93,  92,   0,
  89,  90,  91,  85,  87,  83,  84,  86,  88,  98,
  79,  80,  81,  82,  75,  76,  77,  78,  74,  96,
  97,   0,  95,  94,  93,  92,   0,  89,  90,  91,
  85,  87,  83,  84,  86,  88,   0,  79,  80,  81,
  82,  75,  76,  77,  78,  74,  96,  95,  94,  93,
  92,   0,  89,  90,  91,  85,  87,  83,  84,  86,
  88,   0,  79,  80,  81,  82,  75,  76,  77,  78,
  74,  96,   0,   0,  94,  93,  92,   0,  89,  90,
  91,  85,  87,  83,  84,  86,  88,   0,  79,  80,
  81,  82,  75,  76,  77,  78,  74,  96,   0,   0,
  93,  92,   0,  89,  90,  91,  85,  87,  83,  84,
  86,  88,   0,  79,  80,  81,  82,  75,  76,  77,
  78,  74,  96, 113,   0,   0,   0,   0,  92, 112,
  89,  90,  91,  85,  87,  83,  84,  86,  88,   0,
  79,  80,  81,  82,  75,  76,  77,  78,  74,  96,
   0,   0,   0,   0,   0,   0,   0,   0,  89,  90,
  91,  85,  87,  83,  84,  86,  88,   0,  79,  80,
  81,  82,  75,  76,  77,  78,  74,  96,  23,   0,
   0,   0,   0,   0,   9,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0, 111,
   0,   0,   0, 143, 143, 143, 143,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 171,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0, 111, 201,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 214,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 143,   0,   0,   0,   0,   0,   0,
   0,   0, 237, 237, 237,   0,   0,   0,   0,   0,
  10,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 134, 133, 132,   0, 129, 130, 131, 125, 127,
 123, 124, 126, 128,   0, 119, 120, 121, 122, 115,
 116, 117, 118, 114,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0, 143,   0,
   0,   0,   0,   0,   0,   0,   0, 143,   0, 143,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 237, 237,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0, 201,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0, 237 };
short yypact[]={

-253,-1000,-1000,  77,-1000,-197,  62,-1000,-203,  75,
-1000,-1000, 400, 400, 400, 400, 400, 400, 335,-1000,
1438,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000, 400, 400, 400, 476, 400,
1793, 400, 400, 400, -57, -57, -57, -57, 400, 400,
 400,-311,-1000,-1000,-1000,-1000,-1000,-311,1465, -14,
1465, -15,1410,   9,   4,   4,   4,-1000, 400, 400,
-1000,  73, 400, 400, 400, 400, 400, 400, 400, 400,
 400, 400, 400, 400, 400, 400, 400, 400, 400, 400,
 400, 400, 400, 400, 400, 400,1793, -57, 400, 172,
1158,  22,1131, -12,-1000, -16,  71, 400,  71, 400,
-1000,-1000,  61, 400,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,1596,  71,  71,-212,  96,
  27,-244,-1000,-1000, -57,-214,-221,-272,-231, 604,
 540, 128,-1000,-197,-197,-251, 104,-253,-1000,-1000,
 400, 400,-1000, 400, 400,-1000,-1000,-1000,-1000,1465,
1465, 400, 400,  67,  71,  58,  58,  58,  58,  58,
 260, 260, 260, 260, 646, 646, 646, 646, 646, 646,
 646, 646, 646,1596,1568,1541,1516, 400,-276,  69,
-206,  71,  23,  97, 633, 400,-1000,-1000,-1000, -57,
 400,-1000, 677,  66,-1000, 400,1096, 400, -57, 400,
 -57, 400,  76,-1000, 400, 400,-1000, 400,-1000, 400,
 400, 400,-1000,-277,  68,-1000,  64,  95,  60,-274,
-308,-1000,-197,-251,-1000,-278,-1000,1465,1465,1069,
1465, 730,1042,  66,-1000,-1000, -57, 400,-209,-1000,
-1000, 271,1015,-1000,1465, 400, 988,-1000,1490,-1000,
1465,-1000,1465,-1000,1490,1490,-279,1465,-280,1465,
1465,1465,-1000,-1000,-306,-1000,-1000,-311,-197,-311,
-1000,-1000,-1000, 101,  59,-317, 400, 400,-1000,1465,
 400,-1000, 961,-1000,  91,-1000,-1000,-295,-1000,-327,
-281,-1000,-1000,-1000,-1000,-1000, 400,  57,-1000,-1000,
1465, 730,1465,-1000,-1000,-1000,-1000,-291, 400,-1000,
 709,-1000,-1000,-302,   1,-1000,-1000,-1000,-1000,-1000 };
short yypgo[]={

   0, 103,1888, 224,  56, 223, 221, 219,  46, 210,
   8, 208, 205, 199, 197, 125,   0, 137, 143, 158,
 195, 193, 188, 186, 184, 183, 182, 171, 170, 105,
 133, 166,  72, 215,  63, 163, 160, 117, 112, 156,
 154,  31, 152, 151, 150,  61, 149, 196,  62, 147,
 146,  99, 145 };
short yyr1[]={

   0,   2,   1,   1,   5,   6,   1,   7,   7,   3,
   9,  10,  12,  12,  11,  11,  13,   4,  15,  15,
  17,  17,  14,  14,  14,  14,  14,  14,  14,  14,
  14,  14,  14,  14,  14,  23,  23,  24,  24,  25,
  26,  26,  27,  27,  28,  28,  21,  21,  29,  29,
  30,  30,  31,  31,  34,  34,  34,  34,  34,  34,
  20,  16,  16,  16,  16,  16,  16,  16,  16,  16,
  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,
  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,
  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,
  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,
  16,  16,  16,  16,  18,  18,  19,  19,  19,  19,
  39,  22,  22,  22,  37,  37,  37,  37,  38,  38,
  38,  38,  38,  40,  40,  40,  40,  40,  40,  40,
  40,  40,  40,  40,  40,  40,  40,  40,  40,  40,
  40,  40,  40,  40,  41,  41,  42,  42,  35,  44,
  46,  35,  45,  45,  49,  43,  48,  48,   8,   8,
   8,  47,  47,  50,  36,  51,  51,  51,  52,  52,
  33,  33,  33,  32,  32 };
short yyr2[]={

   0,   1,   1,   2,   0,   0,   5,   1,   2,   7,
   1,   2,   0,   1,   3,   1,   0,   2,   0,   2,
   0,   2,   1,   2,   3,   3,   3,   3,   3,   3,
   2,   1,   1,   1,   1,   2,   0,   1,   2,   7,
   5,   0,   1,   2,   1,   2,   5,   5,   1,   3,
   3,   1,   3,   3,   3,   3,   4,   4,   5,   2,
   3,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   3,   3,   3,   2,   6,   2,   2,
   4,   6,   3,   3,   3,   3,   3,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   4,   3,   2,   2,
   2,   4,   4,   4,   1,   3,   1,   3,   3,   5,
   3,   4,   4,   4,   0,   3,   1,   1,   1,   1,
   2,   3,   4,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   2,   4,   0,
   0,   7,   2,   1,   0,   6,   0,   2,   0,   4,
   4,   1,   3,   1,   4,   1,   2,   3,   3,   4,
   1,   1,   3,   1,   3 };
short yychk[]={

-1000,  -1,  -3,  -4, 256, 321, -13,  59,  -5,  -2,
 257, -14, 324, 322, 323, 318, 319, 329, 330, -20,
 -16, -21, -22,  -2, -25, 295, 258, 260, 259, 326,
 305, 316, 314, -35, -36,  40,  91, 123,  64,  35,
 291, 271, 282, 283, 304, 299, 307, 306, 328, 297,
 312, -43, 266, -41, 311, 320, 262,  59, -16, -18,
 -16, -19, -16, -18, -19, -19, -19, -17, 298, 264,
 -34,  46,  63,  64, 290, 286, 287, 288, 289, 282,
 283, 284, 285, 277, 278, 275, 279, 276, 280, 272,
 273, 274, 270, 269, 268, 267, 291, 265, 123,  40,
 -16, -37, -16, -18, -39, -37, -16,  40, -16, -38,
 -40,  -2,  46,  40, 290, 286, 287, 288, 289, 282,
 283, 284, 285, 277, 278, 275, 279, 276, 280, 272,
 273, 274, 270, 269, 268, -16, -16, -16, -30, -31,
 -32, -33, 295,  -2,  91, -30, -30, -29, -30, -16,
 -16, -16,  -8, 313, 327, -44, -49,  -6,  -8, -15,
  44, 308, -15,  44, 266, -17, -17, -17, -17, -16,
 -16,  -2,  40, -16, -16, -16, -16, -16, -16, -16,
 -16, -16, -16, -16, -16, -16, -16, -16, -16, -16,
 -16, -16, -16, -16, -16, -16, -16, -38, -51, -52,
 -33,  -2, -18, -18, -16, 294,  41,  41,  93, 266,
 294, 125, -16, -16,  -2,  40, -16, 266,  44, 272,
  44, 277, -32, -33, 266, 266, 300, 266, 300, 310,
 309, 308, 325, -10, -11,  -4, -47,  -2, -47, -45,
 -47, -48, 317,  40,  -1,  -9, -10, -16, -16, -16,
 -16, -16, -16, -16, -50, 303,  59, 264, -34, 125,
  41, 294, -16, -29, -16,  44, -16,  41, -16, -30,
 -16, -33, -16,  93, -16, -16, -10, -16, -10, -16,
 -16, -16, -10, -42, 303, -12,  59,  59,  44,  59,
 296, -48, -47, -45,  -7, 303, 266,  41, -51, -16,
 264,  41, -16,  41, -18,  41, -27, 303, -28, 303,
 -26, -41,  -4,  -8, -47,  -8, -46,  41,  59, 321,
 -16, -16, -16,  41,  41, 306, 328, -23, 302, 301,
 -16,  59, -24, 303, -16, -10, 266, 312, 325, -10 };
short yydef[]={

  16,  -2,   2,   0,   4,   0,   0,   3,   0,   0,
   1,  17,  22,   0,   0,   0,   0,   0,  20,  31,
  32,  33,  34,  61,  62,  63,  64,  65,  66,  67,
  68,  69,  70,  71,  72,   0, 124, 124,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 168, 159, 164, 154, 155,   5, 168,  23,  18,
 114,  18, 116,  20,  20,  20,  20,  30,   0,   0,
  79,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 114, 126, 127,   0,  76,   0,  78,   0,
 128, 129,   0,   0, 133, 134, 135, 136, 137, 138,
 139, 140, 141, 142, 143, 144, 145, 146, 147, 148,
 149, 150, 151, 152, 153, 108, 109, 110,   0,  51,
   0, 183, 180, 181,   0,   0,   0,   0,  48,   0,
   0,   0,  16,   0,   0, 166,   0,  16,  16,  24,
   0,   0,  25,   0,   0,  26,  27,  28,  29,  21,
  60,   0,   0,  -2,  -2,  84,  85,  86,  87,  88,
  89,  90,  91,  92,  -2,  -2,  -2,  -2,  -2,  -2,
  -2,  -2,  -2, 102, 103, 104,  -2,   0,   0, 175,
   0, 181,   0,   0, 114,   0,  59,  73,  74,   0,
   0,  75,   0,  -2, 130,   0,   0,   0,   0,   0,
   0,   0,   0, 183,   0,   0,  16,   0,  16,   0,
   0,   0,  16,   0,  12,  15,   0, 171,   0,   0,
 166, 163,   0, 166,   6,   0,  10, 115,  19, 118,
 117,  80,   0,  -2, 174, 173, 176,   0,   0,  54,
  55,   0,   0, 125, 120,   0,   0, 131, 111,  50,
  52, 184,  53, 182, 112, 113,   0,  49,   0, 121,
 122, 123,  41, 158, 156,  11,  -2, 168,   0, 168,
 160, 162, 167,   0,   0,   7,   0,   0, 177, 178,
   0,  56,   0,  57,   0, 132,  46,  42,  47,  44,
  36, 157,  14, 169, 172, 170,   0,   0,   9,   8,
 119,  81, 179,  58,  77,  43,  45,   0,   0,  16,
   0, 165,  39,  37,   0,  35, 161,  38,  16,  40 };
#ifndef lint
static	char yaccpar_sccsid[] = "@(#)yaccpar 1.6 88/02/08 SMI"; /* from UCB 4.1 83/02/11 */
#endif

# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps>= &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 1:
{yyval = New_AST(T_Id, (AST_Ptr)str((char*)yypvt[-0]), ANULL);} break;
case 2:
{parsed = yypvt[-0]; return  0;} break;
case 3:
{parsed = yypvt[-1]; return  0;} break;
case 4:
{ fix_error(); } break;
case 5:
{ need_clear = false; } break;
case 6:
{ yyval = yypvt[-0]; } break;
case 9:
{yyval = New_AST(T_Prog, yypvt[-3], yypvt[-2]);} break;
case 11:
{yyval = Rev_Seq(yypvt[-1]);} break;
case 14:
{yyval = New_AST(0, yypvt[-0], yypvt[-2]);} break;
case 15:
{yyval = New_AST(0, yypvt[-0], ANULL);} break;
case 16:
{ yyval = New_AST(T_Source, Source_List,ANULL); Source_List = yyval; } break;
case 17:
{ yyval = yypvt[-1]; ast_2(yypvt[-1])= yypvt[-0];} break;
case 18:
{yyval = ANULL;} break;
case 19:
{ yyval=yypvt[-0]; } break;
case 20:
{yyval = ANULL;} break;
case 21:
{ yyval=yypvt[-0]; } break;
case 22:
{yyval = New_AST(T_Return,ANULL,ANULL);} break;
case 23:
{yyval = New_AST(T_Return,yypvt[-0],ANULL);} break;
case 24:
{yyval = New_AST(T_Read, yypvt[-1], yypvt[-0]);} break;
case 25:
{yyval = New_AST(T_ReadF, yypvt[-1], yypvt[-0]);} break;
case 26:
{yyval = New_AST(T_Print, yypvt[-1], yypvt[-0]);} break;
case 27:
{yyval = New_AST(T_PrintF, yypvt[-1], yypvt[-0]);} break;
case 28:
{yyval = New_AST(T_PrintF, yypvt[-1], yypvt[-0]);} break;
case 29:
{yyval = New_AST(T_PrintF, 
			  New_AST(0,
				  New_AST(T_Om,ANULL,ANULL),
				  New_AST(0, New_AST(T_String,
						     (AST_Ptr)str("\n"),ANULL),
					  yypvt[-1])),
			  yypvt[-0]);} break;
case 30:
{yyval = New_AST(T_PrintF, 
			  New_AST(0,
				  New_AST(T_Om,ANULL,ANULL),
				  New_AST(0, New_AST(T_String,
						     (AST_Ptr)str("\n"),ANULL),
					  ANULL)),
			  yypvt[-0]);} break;
case 32:
{yyval = New_AST(CALL, yypvt[-0], ANULL); } break;
case 35:
{yyval = yypvt[-0];} break;
case 36:
{yyval = ANULL; } break;
case 39:
{yyval = New_AST(T_If, New_AST(0, yypvt[-5], yypvt[-3]),
			    New_AST(0, Rev_Seq(yypvt[-2]), yypvt[-1]) ); } break;
case 40:
{yyval = New_AST(T_Elseif, New_AST(0, yypvt[-2], yypvt[-0]),
				yypvt[-4]); } break;
case 41:
{yyval = ANULL; } break;
case 46:
{yyval = New_AST(T_For, yypvt[-3], yypvt[-1]);} break;
case 47:
{yyval = New_AST(T_While, yypvt[-3], yypvt[-1]);} break;
case 48:
{yyval = New_AST(ITERS, yypvt[-0], ANULL);} break;
case 49:
{yyval = New_AST(ITERS, yypvt[-2], yypvt[-0]);} break;
case 50:
{yyval = Spread(yypvt[-2], yypvt[-0]);} break;
case 51:
{yyval = Spread(yypvt[-0], ANULL);} break;
case 52:
{yyval = New_AST(T_In, yypvt[-2], yypvt[-0]);} break;
case 53:
{yyval = New_AST(MAP, yypvt[-2], yypvt[-0]);} break;
case 54:
{yyval = New_AST('{', yypvt[-1], ANULL);} break;
case 55:
{yyval = New_AST('(', yypvt[-1], ANULL);} break;
case 56:
{yyval = New_AST(T_Dotdot, yypvt[-2], ANULL);} break;
case 57:
{yyval = New_AST(T_Dotdot, ANULL, yypvt[-1]);} break;
case 58:
{yyval = New_AST(T_Dotdot, yypvt[-3], yypvt[-1]);} break;
case 59:
{yyval = New_AST(CALL, ANULL, ANULL);} break;
case 60:
{yyval = New_AST(T_Gets, yypvt[-2], yypvt[-0]);} break;
case 63:
{yyval = New_AST(T_Missing, ANULL, ANULL);} break;
case 64:
{yyval = New_AST(T_Integer, (AST_Ptr)str((char*)yypvt[-0]),
					   ANULL);} break;
case 65:
{yyval = New_AST(T_Real,    (AST_Ptr)str((char*)yypvt[-0]),
					   ANULL);} break;
case 66:
{yyval = New_AST(T_String,  (AST_Ptr)str((char*)yypvt[-0]),
					   ANULL);} break;
case 67:
{yyval = New_AST(T_True,  ANULL, ANULL);} break;
case 68:
{yyval = New_AST(T_False, ANULL, ANULL);} break;
case 69:
{yyval = New_AST(T_Om,    ANULL, ANULL);} break;
case 70:
{yyval = New_AST(T_Newat, ANULL, ANULL);} break;
case 73:
{yyval = yypvt[-1];} break;
case 74:
{yyval = New_AST('[', yypvt[-1], ANULL); } break;
case 75:
{yyval = New_AST('{', yypvt[-1], ANULL); } break;
case 76:
{yyval = New_AST(DEREF, yypvt[-0], ANULL); } break;
case 77:
{yyval = New_AST(DEREF, yypvt[-3], yypvt[-1]); } break;
case 78:
{yyval = New_AST('#', yypvt[-0], ANULL); } break;
case 79:
{yyval = New_AST(SELECTOR, yypvt[-1], yypvt[-0]);} break;
case 80:
{yyval = New_AST(SELECTOR, yypvt[-1],
		    New_AST( '(', New_AST(0, yypvt[-0],
					 New_AST(0, yypvt[-3], ANULL)),
				  ANULL) );} break;
case 81:
{yyval = New_AST(SELECTOR, yypvt[-2],
		    New_AST( '(', New_AST(0, yypvt[-0],
					 New_AST(0, yypvt[-5], ANULL)),
				  ANULL) );} break;
case 82:
{yyval = New_AST('?',      yypvt[-2], yypvt[-0]);} break;
case 83:
{yyval = New_AST('@',      yypvt[-2], yypvt[-0]);} break;
case 84:
{yyval = New_AST(T_Exp,    yypvt[-2], yypvt[-0]);} break;
case 85:
{yyval = New_AST(T_Times,  yypvt[-2], yypvt[-0]);} break;
case 86:
{yyval = New_AST(T_Slash,  yypvt[-2], yypvt[-0]);} break;
case 87:
{yyval = New_AST(T_Mod,    yypvt[-2], yypvt[-0]);} break;
case 88:
{yyval = New_AST(T_Div,    yypvt[-2], yypvt[-0]);} break;
case 89:
{yyval = New_AST(T_Plus,   yypvt[-2], yypvt[-0]);} break;
case 90:
{yyval = New_AST(T_Minus,  yypvt[-2], yypvt[-0]);} break;
case 91:
{yyval = New_AST(T_With,   yypvt[-2], yypvt[-0]);} break;
case 92:
{yyval = New_AST(T_Less,   yypvt[-2], yypvt[-0]);} break;
case 93:
{yyval = New_AST(T_Eq,     yypvt[-2], yypvt[-0]);} break;
case 94:
{yyval = New_AST(T_Ne,     yypvt[-2], yypvt[-0]);} break;
case 95:
{yyval = New_AST(T_Lt,     yypvt[-2], yypvt[-0]);} break;
case 96:
{yyval = New_AST(T_Gt,     yypvt[-2], yypvt[-0]);} break;
case 97:
{yyval = New_AST(T_Le,     yypvt[-2], yypvt[-0]);} break;
case 98:
{yyval = New_AST(T_Ge,     yypvt[-2], yypvt[-0]);} break;
case 99:
{yyval = New_AST(T_In,     yypvt[-2], yypvt[-0]);} break;
case 100:
{yyval = New_AST(T_Notin,  yypvt[-2], yypvt[-0]);} break;
case 101:
{yyval = New_AST(T_Subset, yypvt[-2], yypvt[-0]);} break;
case 102:
{yyval = New_AST(T_And,    yypvt[-2], yypvt[-0]);} break;
case 103:
{yyval = New_AST(T_Or,     yypvt[-2], yypvt[-0]);} break;
case 104:
{yyval = New_AST(T_Impl,   yypvt[-2], yypvt[-0]);} break;
case 105:
{yyval = New_AST(T_Iff,    yypvt[-2], yypvt[-0]);} break;
case 106:
{yyval = New_AST(T_Insert, yypvt[-1], New_AST(0, yypvt[-3], yypvt[-0]) );} break;
case 107:
{yyval = New_AST(T_Insert, yypvt[-1], 
		      New_AST(0, ANULL, yypvt[-0]) );} break;
case 108:
{yyval = New_AST(T_Not, ANULL, yypvt[-0]);} break;
case 109:
{yyval = New_AST(T_Plus, ANULL, yypvt[-0]);} break;
case 110:
{yyval = New_AST(T_Minus, ANULL, yypvt[-0]);} break;
case 111:
{yyval = New_AST(T_Exists, yypvt[-2], yypvt[-0]);} break;
case 112:
{yyval = New_AST(T_Choose, yypvt[-2], yypvt[-0]);} break;
case 113:
{yyval = New_AST(T_Forall, yypvt[-2], yypvt[-0]);} break;
case 114:
{yyval = New_AST(0, yypvt[-0], ANULL); } break;
case 115:
{yyval = New_AST(0, yypvt[-0], yypvt[-2]); } break;
case 116:
{yyval = New_AST(0, New_AST(T_Om,ANULL,ANULL),
					 New_AST(0,yypvt[-0], ANULL)); } break;
case 117:
{yyval = New_AST(0,yypvt[-0],New_AST(0,yypvt[-2],ANULL)); } break;
case 118:
{yyval = New_AST(0, New_AST(T_Om,ANULL,ANULL),
			 New_AST(0, yypvt[-0], yypvt[-2])); } break;
case 119:
{yyval = New_AST(0,yypvt[-0],New_AST(0,yypvt[-2],yypvt[-4])); } break;
case 120:
{yyval = New_AST(T_Dotdot, yypvt[-2], yypvt[-0]);} break;
case 121:
{yyval= New_AST(T_Frome, yypvt[-2], yypvt[-0]);} break;
case 122:
{yyval= New_AST(T_Fromb, yypvt[-2], yypvt[-0]);} break;
case 123:
{yyval= New_AST(T_From,  yypvt[-2], yypvt[-0]);} break;
case 124:
{yyval = ANULL;} break;
case 125:
{yyval = New_AST(T_St, yypvt[-2], yypvt[-0]);} break;
case 128:
{ yyval = New_AST((int)yyval, ANULL,ANULL); } break;
case 130:
{yyval = yypvt[-0];} break;
case 131:
{yyval = New_AST('(',	yypvt[-1],    ANULL);} break;
case 132:
{yyval = New_AST('(',	yypvt[-1],    ANULL);} break;
case 154:
{yyval=(YYSTYPE)T_Func;} break;
case 155:
{yyval=(YYSTYPE)T_Proc;} break;
case 156:
{yyval=(YYSTYPE)T_End; /* presumes |ptr| >= |int| */} break;
case 157:
{yyval = yypvt[-0];} break;
case 158:
{ yyval = make_func(yypvt[-3],yypvt[-2],yypvt[-1],yypvt[-0]); } break;
case 159:
{ yyval = New_AST(T_Source, Source_List,ANULL); Source_List = yyval;
          if( func_level == 0 ) {
	      Func_Tail = Func_List = mcCons(Nil,Nil);
          }
	  func_level++;
        } break;
case 160:
{ yyval = New_AST(0,yypvt[-2],yypvt[-1]); } break;
case 161:
{ func_level --;
	      if( func_level==0 ) Source_Flush();

	      yyval = New_AST(T_Func, 
		     New_AST(T_Source,
			     (func_level==0 ? Func_List : Nil),
			     ast_2(yypvt[-2])), /* ARGS */
		     New_AST(0, ANULL, /* DCLS */
			     New_AST(0, 
				     yypvt[-5], /* First line */
				     New_AST(0, New_AST(T_Return, yypvt[-1], ANULL),
					     ANULL))));
	      if(func_level==0) {
		  Func_List = Nil;
	      }
	    } break;
case 162:
{ yyval = New_AST(0,yypvt[-1],yypvt[-0]); } break;
case 163:
{ yyval = New_AST(0,ANULL,yypvt[-0]); } break;
case 164:
{ yyval = New_AST(T_Source, Source_List,ANULL); Source_List = yyval;
          if( func_level == 0 ) {
	      Func_Tail = Func_List = mcCons(Nil,Nil);
          }
	  func_level++;
        } break;
case 165:
{ yyval = New_AST((int)yypvt[-5],yypvt[-4],yypvt[-2]); /* presumes |ptr| >= |int| */ } break;
case 166:
{ yyval = ANULL; } break;
case 167:
{ yyval = yypvt[-0]; } break;
case 168:
{ yyval = ANULL; } break;
case 169:
{ yyval = New_AST(T_Local, yypvt[-2], yypvt[-0]); } break;
case 170:
{ yyval = New_AST(T_Value, yypvt[-2], yypvt[-0]); } break;
case 171:
{ yyval = New_AST(0, yypvt[-0], ANULL); } break;
case 172:
{ yyval = New_AST(0, yypvt[-2], yypvt[-0]  ); } break;
case 174:
{ yyval = New_AST(T_Where, yypvt[-3], yypvt[-1]); } break;
case 175:
{ yyval = New_AST(0, yypvt[-0], ANULL); } break;
case 176:
{ yyval = New_AST(0, yypvt[-1], ANULL); } break;
case 177:
{ yyval = New_AST(0, yypvt[-2], yypvt[-0]); } break;
case 178:
{ yyval = New_AST( T_Gets, yypvt[-2],yypvt[-0]); } break;
case 179:
{ yyval = New_AST(SELECTOR, New_AST(SELECTOR, yypvt[-3], yypvt[-2]), yypvt[-0]); } break;
case 180:
{yyval = New_AST(T_Missing, ANULL, ANULL);} break;
case 182:
{yyval = New_AST('[', yypvt[-1], ANULL); } break;
case 183:
{ yyval = New_AST(0, yypvt[-0], ANULL); } break;
case 184:
{ yyval = New_AST(0, yypvt[-0], yypvt[-2]); } break;
		}
		goto yystack;  /* stack new state and value */

	}
