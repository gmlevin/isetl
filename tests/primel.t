program primes;
    print "Enter N:";
    read N;
    P := [];
    for x in [2..N] do
        if forall y in P | x mod y /= 0
	    then P := P + [x];
	    end;
	end;
    
    print P;
    end;

100;
