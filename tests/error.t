!verbose on
x + ~ ;
for i = s(3) do print i; end;
for i = 3    do print i; end;
for [i,i] in {[1,2],[3,4]} do print i; end;
for i = s() do print i; end;

if true then return 5; end;
return 4;
read x;
a := 5;
s := "abcd";
s(2..3) := 5;
t := [1,2,3,4];
t(1..3) := 5;
m := {};
m{3} := 4;
m{3} := {1,2,3};
m{3};
m(3);
s(3..10);
s(3..1);
a := {true..false};
a := {1, 3.5 .. 7};
for i in [1.."abc"] do print i; end;
for i in [1, 3 .. "abc"] do print i; end;
for i=s{j} do print [i,j]; end;
s{4};
t{4};
om{4};
4/0;
4 < true;
{1..3} <= {1..10};
[a,b,c] := 5;
#4;
m(3) := 4;
t(-5) := 4;
x := 4;
take y from x;
for i in 3 do print i; end;
$notice that 'and' and 'or' only evaluate as much as needed
$ this can be useful, but it can also yield odd results
true and 3;
if true and 3 then print 1; else print 2; end;
3 and true;
3 or true;
3 impl true;
not (2+3);
{1..3} + 5;
om + om;
{1..3} - (1+4);
om - om;
-{1..3};
[1,2,3] less 5;
5 with {1..3};
"abc" * "cde";
5 * {1..3};
5 mod 0;
5 mod -3;
5 mod true;
true mod false;
3.5 div 4.5;
{1..3} / {2};
{1..3} / 2;
{1..3} ** 2;
5 in "abc";
5 in 7;
3 subset 5;
read x from om;
print 3 to om;
f := func(i); return [1..i]; end;
f(4);
f(3) := [1..5];
f(4);
f(3);
f(4)(2) := 10;

$ not wrong, but maybe unexpected
+3;
N := 5;
f := func(i); value N; return i+N; end;
N := 1000;
f(4);
