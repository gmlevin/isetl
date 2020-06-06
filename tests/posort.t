program top_sort;
    print "Enter a partial order as a set of pairs";
    read po;
    nodes := domain(po) + image(po);
    print ["nodes", nodes];

    sort := [];
    while nodes - image(po) /= {} do
	n := arb(nodes - image(po));
	sort := sort with n;
	nodes := nodes less n;
	po{n} := om;
    end;

    print ["sort", sort];
end;


{ [1,2], [1,3], [2,4], [2,5], [3,5], [3,6], [4,8], [5,7], [6,7], [7,8] };



$ breadth first (ready is maintained as a queue)
program top_sort;
    print "Enter a partial order as a set of pairs";
    read po;
    nodes := {x: [x,~] in po} + {y: [~,y] in po};
    print ["nodes", nodes];

    $ op = inverse of po
    op := { [y,x] : [x,y] in po };

    $ count(y) = # predecessors of y
    $ ready    = { y : count(y) = 0 or OM }
    count := { [y, #x] : x=op{y} };
    ready := [ y : y in nodes | count(y) = OM ];

    sort := [];
    while ready /= [] do
	take n fromb ready;
	sort := sort with n;
	for y in po{n} do
	    count(y) := count(y) - 1;
	    if count(y) = 0 then
		ready := ready with y;
	    end;
	end;
    end;

    print ["sort", sort];
end;


{ [1,2], [1,3], [2,4], [2,5], [3,5], [3,6], [4,8], [5,7], [6,7], [7,8] };
