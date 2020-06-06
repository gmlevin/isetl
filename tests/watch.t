f := func(i); return f(i-1)*i; end;
f(0) := 1;
!watch f
f(4);

f := {};
f(3) := 4;
f(4) := 5;
f;


!unwatch f
f := func(i); f(i) := f(i-1) + f(i-2); return f(i); end;
f(1) := 1;
f(2) := 1;
!watch f
f(6);
f(7);
!unwatch f
