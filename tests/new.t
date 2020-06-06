f := func(a,b); return a-b; end;
g := [1,2,f,4];

$ may use parenthesized expressions as infix operators
(5 .f 3) = 2;
(5 .(g(3)) 3) = 2;

$ an optional period is permitted after %
(%f[5,4,3,2,1]) = -5;
(%.f[5,4,3,2,1]) = -5;
(%(g(3))[5,4,3,2,1]) = -5;
(%.(g(3))[5,4,3,2,1]) = -5;

$ semicolon at the end of a list of statements is optional
for i in [1..3] do
   printf i;
   printf i+5, '\n'
end;

$ introducing the if-expression.  else-part is required.  single expressions 
$ in each part.

f := func(i); return
       if i <= 0 then 1
       else           i*f(i-1)
       end if
     end;
f(-4);
f(4);

'single quotes may be used for strings';
'such strings may contain double quotes (")'; 