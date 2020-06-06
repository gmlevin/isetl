   gt:={[i,j]: i in [1..5], j in [1..5] | i > j};
   hold := gt;

["gt", gt];
gt(0); gt(2); gt{4};
gt{0}; gt{2};

   gt{4} := {};
gt;

   gt := hold;
   "gt{5} := om";
   gt{5} := om;
gt;

   gt := hold;
   "gt{4} := {7,8,9,10}";
   gt{4} := {7,8,9,10};
gt;

   gt := hold;
"{ [x,y]: y=gt{x} }";
{ [x,y]: y=gt{x} };

   gt := { [[i,j], i>j] : i in [1..5], j in [1..5] };
   "{ [[i,j], i>j] : i in [1..5], j in [1..5] }";
gt;

gt(3,4); gt(4,3);
gt{3,4}; gt{4,3};

"{ [x,y]: y=gt{x} }";
{ [x,y]: y=gt{x} };
"{ [x,y]: y=gt(x) }";
{ [x,y]: y=gt(x) };

"{ x : y=gt(x) | y }";
{ x : y=gt(x) | y };
"{ [i,j] : y = gt(i,j) | y }";
{ [i,j] : y = gt(i,j) | y };

s := { [1,[2,3]] };
s(1)(2) := 5;
s;
