


abs(4);	4;
abs(-4);	4;
abs(0);	0;
abs(4.0);	4.00000;
abs(-4.0);	4.00000;
abs(0.0);	0.00000;
ord("a");	97;
ord("abc");	OM;
ord("");	OM;
float(1);	1.00000;
float(1.4);	1.40000;
float(-4);	-4.00000;
fix(1.2);	1;
fix(1.7);	1;
fix(-1.2);	-1;
fix(-1.7);	-1;
ceil(1.2);	2;
ceil(1.7);	2;
ceil(-1.2);	-1;
ceil(-1.7);	-1;
floor(1.2);	1;
floor(1.7);	1;
floor(-1.2);	-2;
floor(-1.7);	-2;
even( 1);	false;
even(-1);	false;
even( 0);	true;
even( 2);	true;
even(-2);	true;
odd( 1);	true;
odd(-1);	true;
odd( 0);	false;
odd( 2);	false;
odd(-2);	false;
"[random( 10) : i in [1..10]]";
[random( 10) : i in [1..10]];
"[random(-10) : i in [1..10]]";
[random(-10) : i in [1..10]];
"[random(1.0) : i in [1..10]]";
[random(1.0) : i in [1..10]];
"[random(1.5) : i in [1..10]]";
[random(1.5) : i in [1..10]];
"[random({1,3,5,6,8,9}) : i in [1..10]]";
[random({1,3,5,6,8,9}) : i in [1..10]];
"[random([1,3,om,6,8,9]) : i in [1..10]]";
[random([1,3,om,6,8,9]) : i in [1..10]];
"[random('abcdefghijkl') : i in [1..10]]";
[random("abcdefghijkl") : i in [1..10]];
char(ord("a"));	"a";
char(ord("f")-ord("a")+ord("A"));	"F";



