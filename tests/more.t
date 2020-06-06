f := func(); for i in [1..5] do if i = 3 then return; end; end; end;

f();

x := {1,2} with om;
x;
x := {} less 3;
x;

f := func(a,b); if b>0 then return a mod b; end; end;
g := func(a,b); if b>0 then return a - a div b * b - a mod b; end;end;
for x,y in [5,9990,12000,123456789] do
   [a,b] := [x,y];
   print [a mod b >= 0, -a mod b >= 0,
          g(a,b)=0, g(-a,b)=0, a div -b = -(a div b),
          [a,b], a div b, a mod b];
end;




is_atom(newat);
is_boolean(true);
is_integer(1); 
is_number(1);
f := openr("abc");
is_file(f);
close(f);
is_defined(1);
is_om(om);
is_floating(1.0);
is_string("abc");
is_tuple([1,2,3]);
is_set({});
is_map({});
even(1234567890);
odd(1234567891);
fix(1) = 1;
fix(1234567890) = 1234567890;
randomize(om);
randomize(1);
random(1234567890);
domain([10..12]) = {1,2,3};
image([10..12])  = {10..12};
sgn(1) = 1;
sgn(1234567890) = 1;
npow({1,2,3}, 1) = {{1},{2},{3}};
abs(ln(exp(1))-1)<0.0001;
abs(log(10**3)-3)<0.0001;
sqrt(4) = 2;
abs( sin(0.5)/cos(0.5) - tan(0.5)) < 0.0001;
abs( atan(tan(0.25)) - 0.25) < 0.0001;
abs( asin(sin(0.25)) - 0.25) < 0.0001;
abs( acos(cos(0.25)) - 0.25) < 0.0001;
font();
abs( asinh(sinh(0.5)) - 0.5) < 0.0001;
abs( acosh(cosh(0.5)) - 0.5) < 0.0001;
abs( atanh(tanh(0.5)) - 0.5) < 0.0001;
