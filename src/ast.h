/* $Header: ast.h,v 3.1 90/04/25 16:24:36 gary Locked $ */

extern AST_Ptr Source_List;	       /* List of nodes whose ast_1 should
				        * receive the current source line */
extern int func_level;
extern AST_Ptr Func_List, Func_Tail;   /* list of strings that make 
					* the source of current top level 
					* func */
extern AST_Ptr Live_AST;	       /* list of live AST nodes, linked
				        * by ast_more */
extern AST_Ptr parsed;		       /* receives result of yyparse */
extern AST_Ptr New_AST(INT X AST_PTR X AST_PTR);	/* Create an AST node */
extern AST_Ptr Rev_Seq(AST_PTR);       /* Destructively  reverse rev */
extern void w_Expr(AST_PTR);	       /* Generate code for an Expression */
extern void w_Assign(AST_PTR);	       /* Generate code for an Assignment */
extern IPtr Make_Join(NONE);	       /* Make and return a join code
				        * block */

extern void AST_Reset(NONE);	       /* Reset AST locals */
extern char *str(STR);		       /* copy string to malloc space */

/* ANULL  Nil for AST Structures
 * LNULL  Empty Label
 */
#define ANULL	Nil
#define LNULL	Nil



/* Walk corresponding structures in AST */
extern void w_Expr_List(AST_PTR X BOOL),
        w_For(AST_PTR),
        w_Former(AST_PTR),
        w_Func(AST_PTR),
        w_If(AST_PTR X BOOL),
        w_Insert(AST_PTR),
        w_Iter(AST_PTR X C_LABEL ADDR X C_LABEL ADDR),
        w_LHS(AST_PTR X INT),
        w_LHS_List(AST_PTR X BOOL X INT),
        w_Prog( AST_PTR ),
        w_Quantifier(AST_PTR),
        w_Sel(AST_PTR),
        w_Stmt(AST_PTR X BOOL),
        w_Stmt_List(AST_PTR X BOOL),
        w_Where(AST_PTR),
        w_While(AST_PTR);

extern void Emit_Op(C_INSTR), Free_Var(NONE);
extern void Bound(C_REF);
