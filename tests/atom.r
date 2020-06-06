"Enter an expression with operators +-*/";
"Parentheses may be used for grouping";
"Arguments are single lower case letters";
"a + b * c + d * ( x + y ) * e";
[["root", !13!],
 ["tree",
  {[!1!, "b"], [!2!, "c"], [!3!, "x"], [!4!, "y"], [!5!, ["+", !3!, !4!]],
   [!6!, "e"], [!7!, "d"], [!8!, ["*", !5!, !6!]], [!9!, ["*", !1!, !2!]],
   [!10!, ["*", !7!, !8!]], [!11!, "a"], [!12!, ["+", !9!, !10!]],
   [!13!, ["+", !11!, !12!]]}]];

