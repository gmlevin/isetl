extern IPtr Get_Assoc(C_REF);
extern void Watch(STR);
extern void UnWatch(STR);
extern Bool Watched(INT);

extern void Lock(STR);
extern void UnLock(STR);
extern Bool Locked(INT);

extern char *Id_String(INT);
extern C_Ref find_id(STR);
extern C_Ref Mk_Var(NONE);
extern void Free_Var(NONE);
extern void Print_Id(INT);
extern void Id_Dump(BOOL X BOOL);
extern IPtr Global_Val(INT);
extern void Mark_Globals(NONE);
