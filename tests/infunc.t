program x;
    print "Series and Sums";

    print "Enter the series function, as a function of one variable";
    print "	Use the form " 
	  + "'func(i); return <some expression using i>; end;'";
    read f;

    print "Enter the number of terms, N:";
    read N;

    print ["i", "f(i)", "%+[ f(x): x in [1..i]]" ];
    for i in [1..N] do
	print [i, f(i), %+[ f(x): x in [1..i]] ];
    end;
end;

func(a); return a**2; end;
10;
