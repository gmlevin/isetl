f := func(a,b); if b/=0 then return a mod b; end; end;
g := func(a,b); if b/=0 then return a - a div b * b - a mod b; end;end;
test := func(a,b);
   print [a mod b >= 0, g(a,b)=0, g(a,b), a div -b = -(a div b),
          [a,b], a div b, a mod b];
   end;
for x,y in [5,9990,12000,123456789] do
    test( x, y);
    test(-x, y);
    test( x,-y);
    test(-x,-y);
end;

for i in [-3,0,3], j in [0,1,2,50] do
    print [i,j,i**j];
end;
