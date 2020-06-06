   x := { [i,i**2] : i in [1..10] };
["function", x];
["image", { i : i = x(j) }];
["domain", { j : i = x(j) }];

   x := "abcdefg";
["explode", x, [i: i in x]];
[[i,c]: c=x(i)];

   x:= [10..20];
[[i,j]: j=x(i)];

   x:= [21..40];
x;
x(5..10);
x(15..30);
x(..10);
x(10..);
