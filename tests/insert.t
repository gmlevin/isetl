%+ [1..10] = 55;
%+ [] = OM;
5 %+ [1..10] = 60;
5 %+ [] = 5;
%max [1,5,2,7,3] = 7;
%max [] = OM;
6 %max [1,5,2,7,3] = 7;
6 %max [] = 6;
10 %max [1,5,2,7,3] = 10;

minus := func(a,b); return a-b; end;
minus(4,1) = 3;

%minus [10,4,2,1] = 3;
%minus [] = OM;
10 %minus [4,2,1] = 3;
10 %minus [] = 10;

