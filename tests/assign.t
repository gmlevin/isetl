program test;
    x := 1; y := 0;
    while x<10 do
	if x=4 then a := [1,a]; b:= [2,b];
	elseif x=3 then a := [3,a];
	elseif x=5 then a := [5,a];
	else y := y+1;
	end;
	x := x+1;
    end;
end;

a;
b;
x;
y;
