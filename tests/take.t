s:= {1,4..16};
t:= [1,4..16];

s with 4 = {1,4,7,10,13,16};
s with 3 = {1,3,4,7,10,13,16};
s less 4 = {1,7,10,13,16};
s less 3 = {1,4,7,10,13,16};
s less 1 = {4,7,10,13,16};
s less 16= {1,4,7,10,13};

t with 4;

"take x from s";
take x from s;
[x,s];
"take x frome t";
take x frome t;
[x,t];
"take x fromb t";
take x fromb t;
[x,t];
"take x from t";
take x from t;
[x,t];

s := {};
t := [];

take x from s;
[x,s];
take x frome t;
[x,t];
take x fromb t;
[x,t];
take x from t;
[x,t];
