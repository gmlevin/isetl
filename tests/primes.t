!allocate 500000
    [x: x in [2..100] | forall y in [2..x-1] | x mod y /= 0];
