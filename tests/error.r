
! Syntax: ~ not permitted

! Syntax: Only identifiers can be bound

! Syntax: Bad RHS for Iterator

! Syntax: warning -- id bound twice in same scope
4;
2;

! Syntax: Bad RHS for Iterator

*** if true then return 5; end;

! Error: Return at top level

! Error: Return at top level

! Error: Input must be an expression

*** s(2..3) := 5;

! Error: RHS in string slice assignment must be string

*** t(1..3) := 5;

! Error: RHS in tuple slice assignment must be tuple

*** m{3} := 4;

! Error: RHS in mmap assignment must be set
{1, 2, 3};

! Error -- Bad mapping: multiple images
{[3, 1], [3, 2], [3, 3]}(3);

! Error: Slice upper bound too big

! Error: Slice lower bound too big

*** a := {true..false};

! Error: Bad args in low..high

*** for i in [1.."abc"] do print i; end;

! Error: Bad args in low..high

*** for i in [1, 3 .. "abc"] do print i; end;

! Error: Bad args in low,next..high

*** for i=s{j} do print [i,j]; end;

! Error: Can't mmap string
{"d"};
{4};

! Error -- Bad mapping: 
OM{4};

! Error: Divide by zero

! Error: Bad arguments in:
4 <relation> true;

! Error: Bad arguments in:
{1, 2, 3, 4, 5, 6, 7, 8, 9, 10} <relation> {1, 2, 3};

*** [a,b,c] := 5;

! Error: Bad arguments in:
[a, b, c] := 5;

! Error: Bad arguments in:
 # 4;

*** t(-5) := 4;

! Error -- Bad mapping: 
[1, 2, 3, 4](-5);

*** take y from x;

! Error: Bad arguments in:
y from x;

*** for i in 3 do print i; end;

! Error: Can't iterate over
3;
3;

*** if true and 3 then print 1; else print 2; end;

! Error: Boolean expected
3;

! Error: Boolean expected
3;

! Error: Boolean expected
3;

! Error: Bad arguments in:
3 impl true;

! Error: Bad arguments in:
 not 5;

! Error: Bad arguments in:
{1, 2, 3} + 5;

! Error: Bad arguments in:
OM + OM;

! Error: Bad arguments in:
{1, 2, 3} - 5;

! Error: Bad arguments in:
OM - OM;

! Error: Bad arguments in:
 - {1, 2, 3};

! Error: Bad arguments in:
[1, 2, 3] less 5;

! Error: Bad arguments in:
5 with {1, 2, 3};

! Error: Bad arguments in:
"abc" * "cde";

! Error: Bad arguments in:
5 * {1, 2, 3};

! Error: Bad arguments in:
5 mod 0;
2;

! Error: Bad arguments in:
5 mod true;

! Error: Bad arguments in:
true mod false;

! Error: Bad arguments in:
3.500 div 4.500;

! Error: Bad arguments in:
{1, 2, 3} / {2};

! Error: Bad arguments in:
{1, 2, 3} / 2;

! Error: Bad arguments in:
{1, 2, 3} ** 2;

! Error: Bad arguments in:
5 in "abc";

! Error: Bad arguments in:
5 in 7;

! Error: Bad arguments in:
3 subset 5;
Can't read from: OM;
Can't write to: OM;
[1, 2, 3, 4];
[1, 2, 3, 4];
[1, 2, 3, 4, 5];

*** f(4)(2) := 10;

! Error: Only one level of selection allowed
3;
9;

