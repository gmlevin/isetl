1 in {1,2,3};
10 in {1,2,3};
{3} in {1,2,3};

{1,2,3} subset {-10..10};
{1,2,3} subset {};
{} subset {1,2,3};
{} subset {};

1 < 2;
2 < 1;
1 <= 2;
1 <= 1;
2 <= 1;

1 > 2;
2 > 1;
1 >= 2;
1 >= 1;
2 >= 1;

"abc" < "xy";
"xy" < "abc";
"abc" <= "xy";
"abc" <= "abc";
"xy" <= "abc";

"abc" > "xy";
"xy" > "abc";
"abc" >= "xy";
"abc" >= "abc";
"xy" >= "abc";

"abc" in "";
"" in "abc";
"abc" in "this abc string";

