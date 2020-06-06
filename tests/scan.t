program words;
    letters := "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    print "Enter a string";
    read sentence;
    while sentence /= "" do
	print sentence;

	print [sentence(i..j):
	       c=sentence(i), j in [i.. #sentence]
	       | c in letters
	          and (i=1 or sentence(i-1) notin letters)
		  and sentence(j) in letters
		  and (j=#sentence or sentence(j+1) notin letters)
		  and (forall e in sentence(i..j) | e in letters)
	      ];

	words := [];
	while exists c in sentence | c in letters do
	    i := %min({i: c = sentence(i) | c in letters});
	    sentence := sentence(i..);
	    if exists c in sentence | c notin letters 
		then j := %min({j: c = sentence(j) 
					    | c notin letters})
			    - 1;
		else j := #sentence;
	    end;
	    words := words + [sentence(1..j)];
	    sentence := sentence(j+1..);
	end;

	print words;
	print "Enter a string";
	read sentence;

    end;
end;

"here is a string";
"    leading blanks in this one";
"   trailing blanks here    ";
"  extra    spaces    everywhere     ";
"non-letters break words too...too bad";
"";
