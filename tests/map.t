program capitalize;
    little  := "abcdefghijklmnopqrstuvwxyz";
    capital := "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    cap_of  := { [a,capital(i)] : a=little(i) };
    read word;
    result := "";
    for a in word do
	result := result + cap_of(a) ? a;
    end;
    print result;
end;

"try This 123";
