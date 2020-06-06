number:=5;

first:=[[random(100.0),random(100.0)]: x in [1..number]]; 
second:=[[random(100.0),random(100.0)]: x in [1..number]]; 

f:=func(t1,t2);
 return (%+[x*x: x in t1])*(%+[x*x: x in t2])
       >= abs(%+[t1(i)+t2(i): i in [1..(#t1)]])**2;
end;

print "print f(first(1),second(1));";
print f(first(1),second(1));     
print "print f(first(number),second(number));";
print f(first(number), second(number));

print "print #[i: i in [1..number]| f(first(i),second(i))];";

print #[i: i in [1..number]| f(first(i),second(i))];
