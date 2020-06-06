program quant;
    if forall i in [1..2], j in [1..2], k in [1..2] | i + j + k < 10
    then print "correct";
    else print "wrong";
    end;

    if forall i in [1..2], j in [1..2], k in [1..2] | i + j + k < 5
    then print "wrong";
    else print "correct";
    end;
end;
