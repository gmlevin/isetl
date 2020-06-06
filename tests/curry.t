$ curry(f)(x)(y) = f(x,y)

       curry := func(f);
		    return func(x);
			       return func(y);
					  return f(x,y);
				      end;
			   end;
		end;

$ trans(f)(x,y) = f(y,x)

       trans := func(f);
		    return func(x,y);
			       return f(y,x);
			   end;
		end;

   add := func(x,y); return x+y; end;
   sub := func(x,y); return x-y; end;

   inc := curry(add)(1);
   dec := curry(trans(sub))(1);

["inc(10) = ", inc(10)];
["dec(10) = ", dec(10)];


   compose := func(f,g);
		  return func(x); return f(g(x)); end;
	      end;

compose(dec,inc) (13);
