old := max_line(40);
2**200;

newline := "\n";

printf ["Sun", "Mon", "Tues", "Wed", "Thu", "Fri", "Sat"]
       + 3*[" "] + [1..30]
       : 7*[5] with "\n", "\n";

printf { [i,j,i+j] : i, j in [1..3] }
       : 5*[ [0,"+",0,"=",0], "\t"] with "\n", newline*2;

program add_list;
nl := "\n";
printf "Enter numbers, terminate with any non-number\n";
done := false;
sum := 0;
while not done do
   printf "Next?\t";
   readf x;
   if is_number(x)
      then sum := sum + x;
      else done := true;
      end if;
   end while;
printf "Total is: ", sum:0, nl;
end;
1
2
3
4
5.7
x
max_line(old);
