$ median.test

med := func(s1);
	local t,u;
	t := func(x);
		return {y: y in s1| y<x};
	     end;
	u := func(x);
		return {y: y in s1| y>x};
	     end;
	return {x: x in s1| (#(t(x)) = #(u(x)))};
	end;

for n in [1..7] do
	print med({1..n});
end;


