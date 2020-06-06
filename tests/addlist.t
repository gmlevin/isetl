program add;
   done := false;
   printf "Enter numbers, terminate with non-number\n";
   sum := 0;
   while not done do
      printf "Next?\t";
      readf x;
      if is_number(x) then sum := sum + x;
         else done := true;
         end if;
      end while;
   printf "Sum is: ", sum:0, "\n";
   end;

   1
   2
   3
   4
   5.7
   .
