$ the abbreviated form for iteration over the same set now works

    [ [x,y] : x,y in [1..4] ];
    [ [x,y,z,w] : x,y in [1..2], z,w in [3..4] ];

$ it is now possible to create recursive functions that
$ do not depend on the name of the function remaining unchanged


    fact := f where f := func(n); 
			     if n<=1 
				then return 1; 
				else return n*f(n-1);
			     end;
			end;
	      end;

    fact (3);

    g := fact;
    fact := om;
    g (3);

$ in contrast, consider what happens here

    fact := func(n); 
	     if n<=1 
		 then return 1; 
		 else return n*fact(n-1);
	     end;
	    end;

    fact (3);

    g := fact;
    fact := om;
    g (3);


$ This example shows how a collection of mutually recursive functions
$ can be created, only making the needed function visible.

    a := a where a := func(); print "a"; b(); end;
		 c := func(); print "c";      end;
		 b := func(); print "b"; c(); end;
		 end;

    a();


$ the following example creates a function that has access to a hidden
$ variable.  this allow you to easily simulate many of the object
$ oriented concepts

    counter := c where c := func(); last := last+1; return last; end;
		       last := 0;
		       end;

    counter();
    counter();
    counter();
    counter();
