f := func(a,b); return a-b; end;
g := [1,2,f,4];
(5 .f 3) = 2;
(5 .(g(3)) 3) = 2;
(%f[5,4,3,2,1]) = -5;
(%.f[5,4,3,2,1]) = -5;
(%(g(3))[5,4,3,2,1]) = -5;
(%.(g(3))[5,4,3,2,1]) = -5;
