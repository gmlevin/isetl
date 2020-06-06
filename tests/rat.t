!rational on
expand := func(x,k);
   $ return string representing x with k decimal places
   local rep,n,d;
   [n,d] := [num(x),den(x)];
   if n < 0 then n := -n; rep := "-" else rep := "" end;

   rep := rep + as_string(n div d);
   rep := rep + '.';
   n := n mod d;

   for i in [1..k] do
       n := n*10;
       rep := rep + as_string(n div d);
       n := n mod d;
   end;

   return rep;
end func;


ATAN := |x,n ->%+ [-1**i * x**(2*i+1) / (2*i+1) : i in [0..n]] |;

pi   := | n -> 4 * ( 4*ATAN(1/5,n) - ATAN(1/239,n) ) |;

x := pi(10);
x;
PI := 2*acos(0);
PI-x;

expand(x,20);

x := %+[2**-i : i in [1..20]];
x;
1-x;
x < 1;
not( x > 1 );
not( x = 1 );
not( x < 3/4 );
x > 3/4;
x + 1/2**20;

!rational off
