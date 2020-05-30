CC=clcc
CFILES	=	isetl.c \
		debug.c mp.c globals.c interp.c \
		op_arith.c      op_ctrl.c       op_io.c \
		op_aux.c        op_data.c       op_logic.c \
		manager.c \
		mc_aux.c        mc_set.c         \
		mc_data.c       mc_io.c         mc_stack.c      mc_tuple.c \
		emitter.c \
		init_ops.c  \
		symbol.c ast.c predef.c rev_info.c edit.c \
		signals.c keywords.c mc_tree.c mc_htree.c \
		scanner.c ile.c macmain.c raw.c sort.c
OFILES	=	isetl.o \
		debug.o mp.o globals.o interp.o \
		op_arith.o      op_ctrl.o       op_io.o \
		op_aux.o        op_data.o       op_logic.o \
		mc_aux.o        mc_set.o         \
		mc_data.o       mc_io.o         mc_stack.o      mc_tuple.o \
		manager.o emitter.o \
		init_ops.o  \
		parser.o scanner.o symbol.o ast.o predef.o \
		rev_info.o edit.o signals.o keywords.o mc_tree.o \
		mc_htree.o ile.o macmain.o raw.o sort.o

HFILES	=	debug.h machine.h micro.h manager.h \
		types.h common.h emitter.h \
		op.h instr.h ast.h predef.h mp.h scanner.h

FILES	=	$(CFILES) $(HFILES) Makefile parser.y \
		.isetlrc 

CREATED	=	core $(OFILES) tags Lint y.output

CO	=	co -q
CFLAGS	=	-g
YFLAGS	=	-d
MESSAGE =	no message

sun:

parser.h:	parser.c
parser.c:	parser.y
	yacc $(YFLAGS) parser.y
	sed -f  y.tab.c  >parser.c
	rm y.tab.c
	mv y.tab.h parser.h

$(OFILES):	debug.h	types.h common.h machine.h mp.h
ast.o:		ast.h parser.h instr.h emitter.h manager.h micro.h op.h
debug.o:	micro.h
edit.o:		parser.h
emitter.o:	emitter.h instr.h manager.h op.h micro.h

globals.o:	ast.h instr.h
init_ops.o:	instr.h op.h manager.h micro.h emitter.h
interp.o:	instr.h emitter.h manager.h ast.h op.h micro.h parser.h scanner.h
keywords.o:	parser.h
manager.o:	manager.h micro.h ast.h parser.h
mc_btree.o:	micro.h manager.h instr.h
mc_htree.o:	micro.h manager.h instr.h
mc_list.o:	micro.h manager.h
mc_tree.o:	micro.h manager.h instr.h
mc_aux.o:	micro.h manager.h
mc_set.o:	micro.h manager.h
mc_data.o:	micro.h manager.h  
mc_io.o:	micro.h manager.h   
mc_stack.o:	micro.h manager.h    
mc_tuple.o:	micro.h manager.h
mp.o:		micro.h manager.h
new_mc.o:	micro.h manager.h instr.h
new_ops.o:	instr.h op.h manager.h micro.h emitter.h
op_arith.o op_ctrl.o op_io.o op_aux.o op_data.o op_logic.o:	instr.h op.h manager.h micro.h emitter.h
parser.o:	ast.h scanner.h
predef.o:	manager.h predef.h micro.h
scanner.o:	parser.h scanner.h
symbol.o:	manager.h

isetl:	$(OFILES)
#	ci -l -f -q -m'$(MESSAGE)' rev_info.c
	$(CC) -c rev_info.c
	$(CC) $(CFLAGS) $(OFILES) -lm -lcurses -o isetl
	chmod 755 isetl

#  /* Sun can use  Vax (old BSD used crmode in place of cbreak) */
vaxbsd4.2:	; make  MESSAGE='$(MESSAGE)' \
                  CFLAGS='$(CFLAGS) -DVax -DCRMode'	isetl
vax:		; make  MESSAGE='$(MESSAGE)' CFLAGS='$(CFLAGS) -DVax'	isetl
sun:		; make  MESSAGE='$(MESSAGE)' CFLAGS='$(CFLAGS) -DNewSun'	isetl
gcc:		; make  MESSAGE='$(MESSAGE)' CFLAGS='$(CFLAGS) -DGCC' \
                     	CC='gcc  ' isetl
msdos:		; make CFLAGS='$(CFLAGS) -DMSDOS'	isetl
sys5:		; make CFLAGS='$(CFLAGS) -DSYS5'	isetl
att:		; make CFLAGS='$(CFLAGS) -DATT'	isetl
vms:		; make CFLAGS='$(CFLAGS) -DVMS'	isetl
mac:		; make CFLAGS='$(CFLAGS) -DMac'	isetl


clean:	; rm -f $(CREATED)
tag:	; ctags $(CFILES) $(HFILES) 2> /dev/null > /dev/null
lint:	; lint -a -h  -x -c -DVax $(CFILES) parser.c 
update:	; make -f .RCS.make VERSION=$(VERSION) MESSAGE='$(MESSAGE)' all
checkin:	; ci -f -l$(VERSION) '-m $(MESSAGE)' $(FILES);
		rcs -L $(FILES); make -t isetl

install:
	cp isetl isetl3
	strip isetl3

