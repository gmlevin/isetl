nl := "\n";
tab := "\t";
quote := "\q";
escape := "\\";
single := "'";
print %+ [ char(i): i in [1..127] ];

printf nl,  1,2:5,3:20,nl;
printf nl,  2**50, nl;
printf nl,  2**50:20, nl;
printf nl,  2**50: 10.10, nl;
printf nl,  1.5, 1.5:20, 1.5:10.02, 1.5 : -15.05, nl;
printf nl,  true, false;
printf nl,  om, om:5;
printf nl,  "abc","def", "xyz":5, "xyz":10, nl;
printf nl,  newat, newat, nl;
f := func(i); return 17; end;
printf nl,  sin, f, nl;
f(3) := 12;
printf nl,  f, nl;
rfile := openr("test");
wfile := openw("abc");
afile := opena("xyz");
printf nl,  rfile,wfile,afile, nl;
close(rfile);
printf nl,  rfile, nl;
printf nl,  "{", {}, "}", nl;
printf nl,  "{", {1..5}, "}", nl;
printf nl,  "{", {1..5}:3, "}", nl;
printf nl,  "{", {1..5}:[3,","], "}", nl;
printf nl,  "[", [], "]", nl;
printf nl,  "[", [1..5], "]", nl;
printf nl,  "[", [1..5]:3, "]", nl;
printf nl,  "[", [1..5]:[3,","], "]", nl;

$ print calendar page
printf nl,  3*[" "]+[1..31] : 7*[3] with nl;
printf nl;
