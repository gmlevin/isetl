f := func(a,b); if b>0 then return a mod b; end; end;
g := func(a,b); if b>0 then return a - a div b * b - a mod b; end;end;
for x,y in [5,9990,12000,123456789] do
   [a,b] := [x,y];
   print [a mod b >= 0, -a mod b >= 0,
          g(a,b)=0, g(-a,b)=0, a div -b = -(a div b),
          [a,b], a div b, a mod b];
end;

for i in [-3,0,3], j in [0,1,2,5] do
    print [i,j,i**j];
end;
