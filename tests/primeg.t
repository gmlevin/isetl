$ A generator is a function of no parameters that returns a sequence
$ of values, a new element each time it is invoked.

$ returns generator of [3,5..]

odds :=
    func(); local last;
	last := 1;
	return	func();
		    last := last+2;
		    return last;
		end;
    end;


$ returns generator of all primes >= 3

primes :=
    func();
	return	someprimes( odds() );
    end;


$ returns generator of primes in S, where S is a generator,
$ S is increasing and contains all integers relatively prime to 
$ all x < S(1)

someprimes :=
    func( S );
	local first, rest;
	return	func();
		    if first = OM then
			first := S();
			rest := someprimes( primewrt( first, S ) );
			return first;
		    else
			return rest();
		    end;
		end;
    end;

$ returns generator of elements of S that are prime wrt x

primewrt :=
    func(x,S);
	return	func();
		    local next;
		    next := S();
		    while next mod x = 0 do
			next := S();
		    end;
		    return next;
		end;
    end;


p := primes();

for i in [1..10] do
    print p();
end;
