$assign 'P1'.t SYS$INPUT
$assign 'P1'.c SYS$OUTPUT
$isetl -s -d 
$deassign  SYS$INPUT
$deassign  SYS$OUTPUT
$diff 'P1'.r 'P1'.c
