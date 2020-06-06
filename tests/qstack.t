program quick;

    s := om;
    while s /= [] do
	print "enter set to be sorted";
	read  s;
	print s;

	$work = list of tuples to be sorted and catenated after result
	work   := [ [x : x in s] ];
	result := [];

	while work /= [] do 
	    next := work(1);
	    work := work(2..);

	    if #next <= 1 then
		result := result + next;
	    else
		pivot := next(1);
		work := [ [y: y in next| y < pivot],
			  [pivot],
			  [y: y in next| y > pivot]
			] + work;
	    end;
	end;

	print result;
    end;

end;

[20,19..1];
"qwertyuioplkjhgfdsazxcvbnm";
$ worst case data is sorted
[1..20];
[20,19..1];

$random data is faster
[25, 13, 14, 5, 6, 7, 22, 23, 24, 1, 2, 9, 27, 28, 34, 35, 26, 15,
37, 38, 40, 16, 17, 30, 31, 21, 10, 11, 18, 32, 33, 39, 29, 19, 3,
4, 8, 20, 12, 36];

$of course, strings work too
["there", "is", "hardly", "any", "problem", "with", "using",
"different", "types", "in", "subtle", "because", "the", "operators",
"are", "polymorphic"];

 [];
