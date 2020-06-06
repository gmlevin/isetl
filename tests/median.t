program median;
    while s /= [] do
	print "Enter a set or tuple:";
	read s;
	x := arb({ x :  x in s |
		#[y: y in s | y < x] = #[y: y in s | y > x] } );
	if x /= om
	    then print ["Median of", s, "is", x];
	    else print [s, "has no median"];
	    end;
	end;
    end;


[1..10];
[1..11];
[x**2: x in [1..11]];
[];
