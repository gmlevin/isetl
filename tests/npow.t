xpow := func(n,S);  local item;
            if n <= 0 then
	        return { {} };
	    elseif n > #S then
	        return {};
	    else
	        take item from S;
		return xpow(n,S) union {x with item: x in xpow(n-1,S)};
	    end if;
        end;

N:= 6;

!verbose on

for i in [0..N] do print xpow(i,{1..N}) = npow(i,{1..N}) end;

