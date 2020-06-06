closed :=	func(S, op);
		return forall a, b in S| a .op b in S;
		end;

assoc :=	func(S, op);
		return forall a, b, c in S|
			      a .op (b .op c) = (a .op b) .op c;
		end;

id :=		func(S, op);
		return arb({ i: i in S|
				forall x in S|
					i .op x = x and x .op i = x});
		end;

inv :=		func(S, op, id);
		return {[a,b] : a, b in S| a .op b = id };
		end;

group :=	func(S, op);
		local zero, minus;
		if is_set(S) and (is_map(op) or is_func(op))
		   and closed(S, op) and assoc(S, op) 
		    then
			zero := id(S, op);
			minus := inv(S, op, zero);

			if zero in S and is_map(minus)
				     and domain(minus) = S
			    then return [S,op,zero,minus];
			    end;
		    end;
		end;


I5 := {0..4};
plus := func(a,b); return (a+b) mod 5; end;
G := group(I5,plus);
G(3) = 0 and G(4) = {[i,(5-i) mod 5] : i in I5};
