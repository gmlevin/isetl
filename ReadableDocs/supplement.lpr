



                  A Gentle Guide to Using ISETL
                    A supplement to the Intro
                           Version 2.0

                         Gary Marc Levin
            Dept of CS and Math \ Clarkson University
                        Potsdam, NY 13676
                         (315) 268--2384
    Bitnet:  gary@clutx \ Internet:  gary@clutx.clarkson.edu
                         October 3, 1989


1 Why read this report?

This is a supplement to An Introduction to ISETL, the
documentation that is sent with all ISETL disks and tapes.  That
document, now referred to as simply the Intro, is a complete
language description.  As such it tends to be extremely dense.
It is also merely concerned with ISETL the language, ignoring
questions about how ISETL works on your computer and how one
makes use of ISETL.
The first topic --- how ISETL works on your computer --- is
addressed in section 2.  The second topic --- how to make use of
ISETL --- is addressed in section 3.  A third topic ---
Miscellania about the language --- is addressed in section 4 and
covers material that appears in the Intro, but with examples and
a little more description.
This report was originally written in response to questions
forwarded to me by Ladnor Geissinger of the University of North
Carolina.  As more questions reach me, I will add those of
general interest to the list.  Sending them electronically is
probably fastest, but I'll take any feedback that I can get.

2 Running ISETL on your computer

It is difficult to cover all the possibilities here, because
ISETL has been written so that it will run on many machines.
This means that I have no way of knowing things like memory size,
number of disks, or available software.  I will try to cover
general possibilities, and leave it to you to check with local
machine experts --- usually called gurus --- for more details.       <==
I'll put a mark in the margin like the one near the word guru to
point out these system dependent points.  Instructors may wish to
check these points before the semester begins.

2.1 Creating files

If you want to do more than use ISETL as a calculator, you will
eventually want to create files that ISETL can read.  These files
may be used as input to programs you have written, or they may be
programs to be !included.



                                1


The files should be simple text files, also called ascii files or
unformatted files.  Lines may end with CR, LF, or CR-LF, although
the last case may result in blank lines on some systems.
The problem arises of how to create such a file.

   ? Preferably, don't use a word processor.
     Word processors put lots of extra information into files, so
     that the file can be printed in an attractive format.
     Unfortunately, each word processor has its own way of hiding
     that extra information, and there is no way that ISETL can
     ignore it.  More unfortunately, I have no way of knowing
     what needs to be done to just save the text.  This is a job
     for your local guru.                                            <==

     For instance, on the Mac, MacWrite has an option when you
     are saving to save text only.  If you save text only, it
     tries to protect you by asking you if you want to save the
     file when you exit.  If you say yes, it saves a formatted
     file.  Arrgh!
   ? On Unix and VMS, just use the standard editors --- vi,
     emacs, edt.

   ? On the Mac and MS-DOS, use an editor if you have one.
     Both the Mac and MSDOS now contain built-in editors.  These
     can be used to create files.  See the section on Editors in
     the Intro.

See 3.3 for hints as to how to use an editor while you are using
ISETL.

2.2 Size of files

There is no important limit to the length of a file, but if you
want to see your input as you !include a file, you should take
note of the width of your screen.  Each line is indented one tab.
This means that if you have 80 columns, you should limit lines to
72 columns.  The Macintosh has a smaller screen.
See 3.1 for an explanation of how to watch as input is read from
files.

2.3 Standard input and output
Standard input and output (stdin and stdout, respectively) are
concepts that come from Unix.  stdin is usually the keyboard, and
stdout is usually the screen.  If this were all there was to it,
I would just say keyboard and screen and be done with it, but
there is more.  This extra ability is useful when you want to
collect a terminal session, say to turn in as homework.  (See 3.1
for information about how to do this in detail.)
On Unix and MS-DOS, you can re-direct stdin and/or stdout for any
program.  What follows does not apply to VMS or the Mac.  See
your guru for suggestions as to how you can get the same effect.     <==
On the command line where you type isetl, you can also type
``< afile''.  This means that instead of reading from the
keyboard, stdin is read from afile.  Any filename may be used in


                                2


place of afile.  Similarly, you can type ``> bfile'' and stdout
will go to bfile instead of the screen.  There will be no visible
output to the screen in this case.
The characters ``<'' and ``>'' may be thought of as ``out of''
and ``in to'', respectively.


2.4 Command line arguments --- VMS
Command line arguments and flags may be passed in all versions of
ISETL, except the Mac.  In the case of VMS, you need to do a
little work first.
Define isetl :== $your$disk:[your.dir]isetl.exe in your login.com
and you will be able to use command line arguments.  The leading
$ makes this a foreign command (a VMS term, I am told).  The rest
is the complete path to the executable version of ISETL.

3 Using ISETL

3.1 How to generate a terminal session

The directive ``!record xyz'' begins appending everything that
you type to the file xyz.  This is a good idea when you are
developing new work, or if you want to be able to show your work
to someone.  It includes everything that you typed, except for
commands to edit past lines (see 3.6.2).
When you are finished in ISETL, you can edit xyz using your
favorite editor (see 2.1), deleting errors and dead-ends.  In
particular, remove the errors that precede !edit and the line
with !edit.  This will give you a clean script of what you did.
To create a terminal session where your input and ISETL's
response are interleaved, as you would see at the terminal, use
file redirection to read from xyz and save it.  You need to
insert !echo on at the beginning of your file to tell ISETL that
it should print your input.
Example:
xyz: !echo on
     x:= 3;
     y:= 4;
     x+y;
     x*y;

Command: isetl -s -d <xyz >abc

abc: > x:= 3;
     > y:= 4;
     > x+y;
     7;

     > x*y;
     12;
The ``-s'' on the command line suppresses the header information
that tells you about ISETL.  The ``-d'' tells ISETL that it is
receiving direct input and should not use the interactive line
editor.


                                3


3.2 Indenting rules and multiple ends

The presentation of programs is a matter that often seems to stir
emotions usually reserved for religious questions.  The most
important point to consider is readability.  If you find that you
are often confusing where statements begin and end, you are
probably are using an inappropriate style.
I recommend that you match the beginnings and endings of
statements, and that you use the optional keywords after end when
the program gets long.  See figure 1.

program nothing;
    i:= 0;
    while i < 10 do
        if i mod 3 = 5
        then print i;
        else
            print 2*i;
            print 3*i;
        end if;
        i:= i+1;
    end while;
end program;


                   Figure 1:  A simple example


3.3 Doing two things at once -- ISETL and Editing

Time for the guru again.  This is very system dependent, even to     <==
the point of how much memory is available.
On Unix, there is usually a key to type that temporarily stops
one program, and lets you start a second (ctrl-Z is the usual
choice).  ^ Z also works within ISETL on MSDOS.
On VMS, the best chance is !system.  This is also available under
Unix and MSDOS. The !system directive can be used to execute a
command without leaving ISETL.  ``!system dir'' lists your
directories.  Generally, ``!system xyz'' is the same as executing
xyz at the system prompt, except that ISETL does not lose its
variables in the meanwhile.  See the Intro for details on how to
save memory in small systems to provide enough room to use
!system.

3.4 Printing output
The simplest solution is to write to a file and then print the
file from outside of ISETL.  Other possibilities exist.  See 3.1
and 2.3.


3.5 Formatting output
This is covered in 4--(Q7).




                                4


3.6 ISETL for beginners

I will consider beginners to be those who are generally writing
one-liners, simple expressions for evaluation.

3.6.1 !record

When you begin, it is probably best to have !record history in
your isetl.ini file.  This will keep a copy of what you type in
the file history.  This is useful for showing to others when
something doesn't work.  There is little more frustrating than to
say, ``This is what I did, and it didn't work.''  only to have
the other person try the same thing and have it succeed.  If you
!record everything, you can find out exactly what you did do
(probably not as you remembered it).

3.6.2 !edit
Most systems no longer have or need !edit, it having been
replaced by the interactive line editor.  If your system does not
have ILE, read on (the header announces the presence of ILE). If
you prefer !edit, you can use it by including the -e switch.
When you are typing one-liners, and even three- and four-liners,
you will often get a character wrong and not feel like typing the
whole thing again.  Until the next > prompt appears, you have a
chance to correct your input with !edit.  You will be offered the
input as it is and given the choice of accepting it or modifying
it.

   ? To accept it, just type RETURN, entering a blank line as
     requested.  (The message is !Enter string to replace (blank
     line to execute), which we have shortened in the example.)

   ? To modify it takes two steps.
     (1) Enter a string to be replaced.

     (2) Enter the string to be used as the replacement.
See figure 2.  In the first case, we made an error and corrected
it.  In the second case, we changed our mind and changed the
input to match.  Try typing this to ISETL to see exactly which
parts are your input and which parts are printed by ISETL.

3.6.3 !verbose

I'd recommend turning ``!verbose on''.  This will give more
information in the error messages, at the possible cost of
printing large sets or tuples.  As a beginner, you need all the
information ISETL can give you.

3.7 ISETL for intermediates

An intermediate is someone who is now writing small programs, too
much to type each time you start again.




                                5














>       1+2+3+;
Syntax error, line 1.
Error at or before end of:
1+2+3+;

Enter '!clear' or '!edit'
>>      !edit
1+2+3+;
!Enter string to replace (blank line...)
+;
!Enter replacement
+4;
1+2+3+4;
!Enter string to replace (blank line...)

10;
>       1+2
>>      !edit
1+2
!Enter string to replace (blank line...)
1
!Enter replacement
5
5+2
!Enter string to replace (blank line...)

>>      ;
7;


                      Figure 2:  Using !edit













                                6



>       f := func(x);
>>               return x+3;
>>           end func;
>       !watch i f
!'i' watched
!'f' watched
>       i := f(5);
! Evaluate: f(5);
! f returns: 8;
! i := 8;


                     Figure 3:  Using !watch

3.7.1 !include

Put your programs into files using an editor (see 2.1).  You can
then use ``!include file.x'' to insert file.x as if you had typed
it at the keyboard, but without the strain on the fingers.

3.7.2 !verbose

I'd recommend turning ``!verbose off''.  Most of the time, you
will have enough of an idea what is wrong just from the operation
that got the error.  You can always turn it back on and repeat
the error if needed.

3.7.3 !watch
With big programs can come big errors.  !watch can provide a
trace of important changes that occur in your program:
assignments and function evaluations.
At the top level (as in figure 3), this is not very helpful, but
inside a larger program, this will let you see each time f is
called, what its arguments were, and what it returned.  You also
see whenever i is assigned a value.

3.7.4 Misspelled identifiers

As programs get larger, there are more chances for things to go
wrong.  A common mistake that makes you appreciate Pascal's
pickiness is misspelling an identifier.  Any identifier is legal,
so there are no syntax errors here.  The problem is that if you
misspell the name on the left of an assignment, the variable of
interest is undefined and if you misspell the name in an
expression, the misspelling is undefined.  (Let's not think about
the problems when half the time you spell it one way and half the
time the other.)
The directive !ids lists identifiers that you have used that are
defined.  !oms lists those identifiers that you have used that
are undefined (have the value om).  From these lists, you can
often find a name that you didn't expect and thereby track down
the problem.



                                7


3.7.5 Debugging

Debugging is described in detail in the Intro.

3.8 ISETL for experts

Experts tend to find their own way of working.  Much of the
decision depends on what tools are available and how comfortable
you are with your computer, separate from ISETL.
I find that I tend to use ISETL more in the style of a compiler,
editing files and having ISETL read them in using the command
line form or !include, only typing short expressions at the
keyboard for testing or evaluating functions.
Being on Unix, I use the Unix means of stopping ISETL, rather
than !system.
It will be interesting to have others report how they make use of
ISETL.
Another option that I am finding useful under Unix is to run
emacs and run ISETL under the emacs shell.  This gives me a full
editor.  If you are an emacs user, just remember to run isetl -d,
so that the ILE is not active also.  The availability of programs
like emacs is the reason I resisted introducing an editor into
ISETL; it really does not belong there.

3.8.1 Garbage collection
If your program seems to run very slowly, getting slower as the
program continues to run, it may be using most of its available
memory.  The directive !gc on will print information about the
available memory and how it is being collected each time ISETL
runs out of space.  If it collects frequently, or the amount of
space reclaimed is small, increase the allocated memory with
!allocate.  If you are at the ISETL memory limit, you can
increase it with !memory, up to the system limits.
On the Mac, under MultiFinder, you may need to adjust the memory
usage information in the Info window.  See a Mac guru.               <==


4 Questions about the language
Q1:   Exactly when does one need to put in ``;'' and what is it
     telling ISETL?

A1:   This is probably the most commonly asked question about any
     programming language that I know of, with the exceptions of
     Lisp and Fortran which don't use semicolons.
     Fortunately, the answer is easy for ISETL; semicolons go
     after every statement.  The semicolon tells ISETL that you
     are finished with the statement.  At the prompt, where you
     can simply type expressions, the importance of the semicolon
     can be easily seen.  If statements and expressions ended at
     the end of the line, like Fortran, the following would not
     work, instead printing 6 after the first line.





                                8


         >       1+2+3
         >>      +4;
         10;


     Bowing to popular pressure, I have made the final semicolon
     in a list of statements optional.  This means that Pascal
     programmers will find that their intuition works fine.  All
     previously correct programs still work.

Q2:   How is x+y; different from print x+y; ?

A2:   This depends on where you write them.
       ? At the prompt, an input that is just an expression
         implies that you want the value printed.  Therefore, at
         the top level, there is no difference.

       ? Inside functions or programs, x+y; simply evaluates x+y,
         producing no output.  Only print x+y; would let you see
         the result.

Q3:   If I assign an expression to B, does ISETL evaluate the
     expression immediately and just store the calculated value
     in B, or does it wait to evaluate the expression until we
     enter B; ?

A3:   ISETL is a value-oriented language and as such you should
     think of expressions as being evaluated when they are used.
     Therefore, the expression assigned to B is evaluated and the
     value stored.
Q4:   What is the difference between program and func?

A4:   A program is a collection of statements to be executed.  It
     has no name, takes no arguments, executes once.  Moreover,
     there is no object in ISETL that corresponds to a program
     --- that is, you can't store a program in a variable or as
     an element of a set.
     A func is an ISETL object, and as such can be used in many
     more ways.

     The program is somewhat like the human appendix, a feature
     left over from the evolutionary predecessor, SETL.  It does
     serve a purpose until such time as you are ready to deal
     with funcs.  Consider the difference between the following
     two sequences of input.











                                9


         >       program example;
         >>          read x;
         >>          read y;
         >>          print [x,y];
         >>      end program;
         ?       5; 6;
         [5, 6];

         >
         >       read x; 5;
         >       read y; 6;
         >       print [x,y];
         [5, 6];

     With a program, the input follows the program.  With
     separate statements, the input must follow the read.

Q5:   What is pretty-printing?

A5:   When you print using print, the output is generally in the
     form of constant expressions, terminated by semicolons.
     (Generally, because some objects don't have a constant form:
     atoms, files, funcs.)
     The large structured objects, like sets and tuples, are
     printed so that the indentation and line breaks respect the
     structure of the object.  Consider:

         >       [ [i,i**2]: i in [100..104] ];
         [[100, 10000], [101, 10201],
          [102, 10404], [103, 10609],
          [104, 10816]];
     Breaking the line in the middle of a pair would make it
     harder to read, so we try to format automatically.  In
     contrast, see (Q7).  The function max_line(x) sets the
     maximum number of columns of output in prettyprinting to x.
     Here I have set it to 40, to keep lines short.

Q6:   What is the ``backslash convention''?

A6:   Not all characters in the ascii character set have a
     printed image, and some are awkward to type.  When you type
     a character string, you can use the backslash convention to
     enter these special characters.  When prettyprinting (Q5),
     strings are printed in the same fashion.  See (Q7) for uses
     of these special characters.
     Here is a table describing these special characters:










                                10


             \b      backspace
             \f      formfeed (new page)
             \n      newline (prints as CR-LF)
             \q      double quote
             \r      carriage return (CR)
             \t      tab
             \octal  character represented by octal
                      Refer to an ASCII chart for meaning.
             \other  other --- may be any character
                      not listed above.

     In particular, "\\" is a single backslash.  You may type,
     "\"" for double quote, but the pretty printer will print as
     "\q".  ASCII values are limited to '\001' to '\377'.

     >       %+ [char(i): i in [1..127]];
     "\001\002\003\004\005\006\007\b\t\n\013\f"
     +"\r\016\017\020\021\022\023\024\025\026"
     +"\027\030\031\032\033\034\035\036\037 !"
     +"\q#$%&'()*+,-./0123456789:;<=>?@ABCDEF"
     +"GHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijk"
     +"lmnopqrstuvwxyz{|}~\177";


Q7:   How do you keep from printing ``;''?  How do you put
     several things on a line with appropriate spacing?

A7:   Two questions with a common answer.  First, why the
     semicolon in the output, then how to get rid of it and
     produce pretty results.
     The semicolon is included in the output so that ISETL output
     may be used as ISETL input.  Where it makes sense, the
     output of print is a constant followed by a semicolon,
     suitable for read.
     You can get output that is more like that of other languages
     by using write.  Use write in place of print (or print...
     to..).  It prints:

       ? numbers without the semicolon
       ? strings without the quotes, semicolon, and the backslash
         convention (see (Q6)).

       ? the elements of tuples and sets, but without any
         punctuation --- no semicolons, no braces or brackets, no
         commas.
     As in Pascal, writeln is like write with a newline written
     at the end.

     Example:

     >       print 12, "ab\ncd",
     >>                 [1,2];
     12;
     "ab\ncd";
     [1, 2];

                                11



     >       write 12, "ab\ncd"; write [1,2];
             12ab
     cd         1         2
     >

     Notice that "\ n" produced a newline and that the second
     write began on the same line as the first ended.  Once you
     go to write it is your responsibility to handle lines.  One
     solution is defining ``newline:= "\ n";'' in your isetl.ini
     file, and then you can print newline as needed.  The other
     solution is to use writeln.

     Each of the types has a default number of columns that it
     uses for output.  You can override the default by putting
     ``:d'' where d is an integer expression that is the minimum
     number of columns to be used.
                       -------------------------
                        Float               20
                        Integer             10
                        String               0
                        Anything else       10

         >       newline := "\n";
         >       write 1,2,3,newline;

         ?????????1?????????2?????????3
         >       writeln 1:3, 2:3, 3:3;

         ??1??2??3

     There are more possibilities for the format, to handle
     floating point numbers and structured data.  See the Intro
     for the complete story.
Q8:   How can I read inputs without needing the semicolon?

A8:   Just as there is printf1, we provide readf.  You can check
     the Intro for the additional possibilities, but the two
     important ones are:

       ? readf x; --- This reads the next sequence of non-white
         characters (skipping blanks, newlines, tabs, etc.).  If
         the sequence can be interpreted as a number, that is
         what is read.  If it cannot be interpreted as a number,
         it is treated as a string.
       ? readf x:-1; --- This reads one character and assigns x
         that one character string.

     Examples:
         >       readf x,y,z,a:-1,b:-1,c:-1;
         123 xyz 123; 12
         >       print [x,y,z,a,b,c];
         [123, "xyz", "123;", " ", "1", "2"];
---------------------------
 1printf is a synonym for write.

                                12


     Notice:

       ? There is no prompt for readf.  You should print your
         own.
       ? 123; was read as a string.

       ? a was assigned " ", the character immediately following
         ``123;''.

Q9:   How can I write a program which, when I choose a specific
     integer n, would make an assignment such as Zn:=0..n-1, that
     is, part of the identifier name depends on my choice of n?
     For example, when I enter MAKEZN(137); it assigns to Z137
     the correct set and also constructs the addition and
     multiplication mod 137 with appropriately chosen identifiers
     (a and m?).
A9:   You can't do this.  You can of course have Z(137):=
     0..137-1; and similarly for a(137) and m(137).  Identifiers
     aren't ISETL objects, so you can't perform operations on
     them.

     I'd recommend that makeZn return a 3-tuple and you assign
     the results as you wish.  It is not a good idea to assign to
     global variables, although it is legal.

         [Z(137),a(137),m(137)] := makeZn(137);

Q10:   What is the purpose of having tuples start at origins
     other than 1?
A10:   A good example is polynomials, where

                                   Xk
                             p(x)=    aixi
                                   i=0
     Here, the tuple a naturally begins at 0, so that the indices
     and exponents correspond.

     You can now define
     p := 0@[1,2,4];
     $ Represents 1 + 2*x + 4*x*x
     eval := :  x,p ?>  %+[ai*x**i :  ai=p(i)] :;

     The definition of eval isn't quite right, because eval(p,0)
     will compute 0**0.  The correct form is left as an exercise
     to the reader.  Also left to the reader is the question why
     one sees the summation in some texts and not others.










                                13
