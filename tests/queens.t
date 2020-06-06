queen := func(n,c);
	if c <= 0 then
		return {[]};
	else
		return { rr with r: rr in queen(n,c-1), r in {1..n} |
				forall cc in {1..#rr} |
					(r /= rr(cc)) and
					((r + c - 1) /= (rr(cc) + cc - 1)) and
					((8 + r - c) /= (8 + rr(cc) - cc))};
	end;
end;


queen(5,5);
