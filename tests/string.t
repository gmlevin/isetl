   x:= "abcdefg";
["abcdefg", x];

["d",x(4)];
["def", x(4..6)];

   x(4) := "";
["abcefg", x];

   x(5) := "ABCDEFG";
["abceABCDEFGg", x];

   x(3..6) := "X";
["abXCDEFGg", x];

   x(3..2) := "QQQ";
["abQQQXCDEFGg", x];


