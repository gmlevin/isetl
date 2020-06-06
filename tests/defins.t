   insert := func(f,id);
		return func(S); local i,part;
			    part := id;
			    for i in S do
				part := f(part,i);
			    end;
			    return part;
		       end;
	     end;

   sum := insert( func(a,b); return a+b; end, 0 );
   prod := insert( func(a,b); return a*b; end, 1 );
   fact := func(N); return prod([1..N]); end;

sum([1..10]);
sum([10,11,12]);
fact(3);
fact(4);
fact(10);
