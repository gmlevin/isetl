$ permset
$
permset := func(s);
		if not is_set(s) then return; end;
		if #(s)=1 then return {[arb(s)]};
		else return {t with x: x in s, t in permset(s less x)};
		end;
	   end;

p := permset({1..4});
#p;
[ {s : s in p | s(1) = i} : i in [1..4] ];

$ setform.test
$

s3 := permset({1,2,3});

pm3 := func(a,b);
		return [a(b(i)): i in [1,2,3]];
	end;

{s .pm3 t : s in s3, t in s3} = s3;

