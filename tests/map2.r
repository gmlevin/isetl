["gt",
 {[2, 1], [3, 2], [3, 1], [4, 3], [4, 2], [4, 1], [5, 4], [5, 3], [5, 2],
  [5, 1]}];
OM;
1;
{1, 2, 3};
{};
{1};
{[2, 1], [3, 2], [3, 1], [5, 4], [5, 3], [5, 2], [5, 1]};
"gt{5} := om";
{[2, 1], [3, 2], [3, 1], [4, 3], [4, 2], [4, 1]};
"gt{4} := {7,8,9,10}";
{[2, 1], [3, 2], [3, 1], [4, 10], [4, 9], [4, 8], [4, 7], [5, 4], [5, 3],
 [5, 2], [5, 1]};
"{ [x,y]: y=gt{x} }";
{[2, {1}], [3, {1, 2}], [4, {1, 2, 3}], [5, {1, 2, 3, 4}]};
"{ [[i,j], i>j] : i in [1..5], j in [1..5] }";
{[[1, 1], false], [[1, 2], false], [[1, 3], false], [[1, 4], false],
 [[1, 5], false], [[2, 1], true], [[2, 2], false], [[2, 3], false],
 [[2, 4], false], [[2, 5], false], [[3, 1], true], [[3, 2], true],
 [[3, 3], false], [[3, 4], false], [[3, 5], false], [[4, 1], true],
 [[4, 2], true], [[4, 3], true], [[4, 4], false], [[4, 5], false],
 [[5, 1], true], [[5, 2], true], [[5, 3], true], [[5, 4], true],
 [[5, 5], false]};
false;
true;
{false};
{true};
"{ [x,y]: y=gt{x} }";
{[[1, 1], {false}], [[1, 2], {false}], [[1, 3], {false}],
 [[1, 4], {false}], [[1, 5], {false}], [[2, 1], {true}],
 [[2, 2], {false}], [[2, 3], {false}], [[2, 4], {false}],
 [[2, 5], {false}], [[3, 1], {true}], [[3, 2], {true}], [[3, 3], {false}],
 [[3, 4], {false}], [[3, 5], {false}], [[4, 1], {true}], [[4, 2], {true}],
 [[4, 3], {true}], [[4, 4], {false}], [[4, 5], {false}], [[5, 1], {true}],
 [[5, 2], {true}], [[5, 3], {true}], [[5, 4], {true}], [[5, 5], {false}]};
"{ [x,y]: y=gt(x) }";
{[[1, 1], false], [[1, 2], false], [[1, 3], false], [[1, 4], false],
 [[1, 5], false], [[2, 1], true], [[2, 2], false], [[2, 3], false],
 [[2, 4], false], [[2, 5], false], [[3, 1], true], [[3, 2], true],
 [[3, 3], false], [[3, 4], false], [[3, 5], false], [[4, 1], true],
 [[4, 2], true], [[4, 3], true], [[4, 4], false], [[4, 5], false],
 [[5, 1], true], [[5, 2], true], [[5, 3], true], [[5, 4], true],
 [[5, 5], false]};
"{ x : y=gt(x) | y }";
{[2, 1], [3, 1], [3, 2], [4, 3], [4, 2], [4, 1], [5, 1], [5, 2], [5, 4],
 [5, 3]};
"{ [i,j] : y = gt(i,j) | y }";
{[2, 1], [3, 2], [3, 1], [4, 1], [4, 3], [4, 2], [5, 3], [5, 4], [5, 2],
 [5, 1]};
{[1, [2, 5]]};

