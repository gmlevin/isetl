.SUFFIXES: .c .r .t
.IGNORE:

.t.r:
	isetl -s -d <$*.t > $*.r 
	chmod a-w $*.r $*.t

.t.c:
	time isetl  -s -d <$*.t | diff - $*.r
#	mv gmon.out $*.mon

tests:	; make `ls *.t | sed s/\\\.t/.c/`

results: ; make `ls *.t | sed s/\\\.t/.r/`
