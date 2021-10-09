# ISETL Resurrected
This is code that was last touched in 1990.
As such, there are still some issues trying to get it to run correctly on
modern versions of Linux.
I am not trying to get it to run on other platforms.

* There are the source files for documents in Docs, written in TeX.
There are lineprinter approximations in ReadableDocs. Again, trying to get ancient TeX
to format is a task for a later time.

* There are test programs in tests. The idea there is that running isetl on `*.t` should produce `*.r`.
Currently there a few cases that fail. I am working on tracking down the issues.

* Finally, there are many sample programs in samples.
   * The top level of the directory are the more straight forward examples.
   * Lower directories have more specialized programs.
   * Some of them were intended to work with graphics output, which is currently not supported.

* See howto.ubuntu for building instructions


## Issues
* Some ISETL programs fail either assertions or core dump.
* The command line editor is not working with the modern version of curses.
* No support for Mac or Windows.
* No support for graphics.