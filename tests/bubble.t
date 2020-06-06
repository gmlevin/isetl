program sort;
    while list /= [] do
	print "Enter tuple to sort";
	read list;
	print ["original list", list];
	swap := { i:  i in [1..#list-1] | list(i) > list(i+1) };
	while swap /= {} do
	    for i in swap do
		[ list(i), list(i+1) ] := [ list(i+1), list(i) ];
	    end;
	    swap := { i:  i in [1..#list-1] | list(i) > list(i+1) };
	end;
	print ["sorted list", list];
    end;
end;
[20,19..1];
["this", "is", "a", "test", "to", "show", "that", "subtle", "is",
"polymorphic"];
[];
