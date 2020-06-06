       quick := func(list);
		    local i,p;

		    if #list <= 1 then return list;
		    else
			p := list(#list div 2);
			return quick([i: i in list | i < p])
			       + ([i: i in list | i = p])
			       + quick([i: i in list | i > p]) ;
		    end;
		end;

    quick([20,19..1]);
    quick("qwertyuioplkjhgfdsazxcvbnm");
$ worst case data is sorted
quick([1..20]);
quick([20,19..1]);

$random data is faster
quick([25, 13, 14, 5, 6, 7, 22, 23, 24, 1, 2, 9, 27, 28, 34, 35, 26, 15,
37, 38, 40, 16, 17, 30, 31, 21, 10, 11, 18, 32, 33, 39, 29, 19, 3,
4, 8, 20, 12, 36]);

$of course, strings work too
quick(["there", "is", "hardly", "any", "problem", "with", "using",
"different", "types", "in", "subtle", "because", "the", "operators",
"are", "polymorphic"]);
