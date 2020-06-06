program tree_to_dag;
    $represent a graph using mapping from atom to [op,left,right]

    print "Enter an expression with operators +-*/";
    print "Parentheses may be used for grouping";
    print "Arguments are single lower case letters";

    read expr;
    print expr;

    multipliers := "*/";
    adders      := "+-";
    vars        := "abcdefghijklmnopqrstuvwxyz";

    operands := [];		$ operand stack
    operators := [];		$ operator stack
    tree	:= {};		$ graph representing the expression

    for c in expr | c /= " " do
	if c in vars   then
	    operands := [c] + operands;

	elseif c in adders then
	    while operators(1)?" " in multipliers do
		left := newat;
		right := newat;
		[operators,      operands, tree(left), tree(right)]
		:= [operators(2..), 
		    [ [operators(1),left,right]] + operands(3..),
		    operands(2),
		    operands(1) ];
	    end;
	    operators := [c] + operators;

	elseif c in multipliers then
	    operators := [c] + operators;

	elseif c = "("   then
	    operators := [c] + operators;

	elseif c = ")"   then
	    while operators(1) /= "(" do
		left := newat;
		right := newat;
		[operators,      operands, tree(left), tree(right)]
		:= [operators(2..), 
		    [ [operators(1),left,right]] + operands(3..),
		    operands(2),
		    operands(1) ];
	    end;
	    operators := operators(2..);

	else
	    print [c, "skipped"];
	end;
    end;

    while operators /= [] do
	left := newat;
	right := newat;
	[operators,      operands, tree(left), tree(right)]
	:= [operators(2..), 
	    [ [operators(1),left,right]] + operands(3..),
	    operands(2),
	    operands(1) ];
    end;

    root := newat;
    tree(root) := operands(1);

    print [["root", root], ["tree", tree]];
end;

"a + b * c + d * ( x + y ) * e";
