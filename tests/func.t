$ test of functions
x := om; y := om; a := om; b := om;  i := om;

       x:= func(); return 1; end;
    x();

       x:= func(); y:= 3; end;
    y;
    x();
    y;

       x:= func(a); y:= a; end;
    a; x(3); a; y;

       x:= func(a); local b; b:= a; y:= a; end;
    b; x(5); b; y;

       x:= func(a); return [i: i in [1..a] | i mod 2 = 0]; end;
       i:= 0;
    x(10);
    i;

       x:= func(a); local i; return [i: i in [1..a] | i mod 2 = 0]; end;
       i:= 0;
    x(10);
    i;


