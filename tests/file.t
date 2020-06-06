f:= openw("abc");
for i in [1..10] do
    print i to f;
end;
close(f);

f := openr("abc");
t := [];
while not eof(f) do
    read i from f;	$ NB Will read an om, but
    t := t with i;	$ t with om = t
end;
close(f);

print t = [1..10];

f:= opena("abc");
for i in [1..10] do
    print i to f;
end;
close(f);

f := openr("abc");
t := [];
while not eof(f) do
    read i from f;	$ NB Will read an om, but
    t := t with i;	$ t with om = t
end;
close(f);

print t = [1..10] + [1..10];

