   x:= [10..30];
   y:= x;

   x(10..15) := [];
x;

   x:= y;
   x(10..15) := [1..10];
x;

   x:= y;
   x(10..15)(3) := 50;
x;

   x := [1..20];
   y := x;
x;

   x(10..15) := [];
x;

   x := y;
   x(10..15) := [50..60];
x;

   x := y;
   x(10..15)(3) := 45;
x;

   x := y;
   x(10..15)(8) := 70;
x;

   x := [1,2,3,4,OM,OM,OM,8,9,10];
   y := x;
x;
x(4..7);
   x(4..7)(2) := 17;
x;

   x := y;
   x(6..7) := [1];
x;

