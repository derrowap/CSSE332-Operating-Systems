# CSSE332-Operating-Systems
Final Project implementation of an operating system for CSSE332 at Rose-Hulman Institute of Technology

Individual README files can be found within each milestone directory providing a summary of the work accomplished for each.

YouTube Presentation (click image):<br>
[![CSSE 332: Operating Systems Final Project](http://img.youtube.com/vi/FUcKRhdHnDw/0.jpg)](https://youtu.be/FUcKRhdHnDw "CSSE 332: Operating Systems Final Project")

This is a 16-bit bare metal operating system built from scratch. The main components consist of:
  * A boot loader to boot from a floppy disk image
  * A set of system calls that are made using interrupts
  * A file system using inodes
  * The ability to execute a program from a file
  * A command line shell with a minimum of necessary commands (directory listing, type, copy, delete, execute) as well as extra commands created as special features
  * Multiprocessing using the Round-Robin algorithm and basic memory management
  
The project originally designed by Michael Black at America University can be found in his [SIGCSE paper](http://www.rose-hulman.edu/class/csse/csse332/current/project/OS_paper_for_sigcse_2009_final.pdf).

Code is written in ANSI C and compiled with the bcc compiler and as86 assembler.
Bochs is a program we used to test the operating system and try it out ourselves.

All code follows the ANSI C standard as well as the [Google C++ style](http://google.github.io/styleguide/cppguide.html) using the provided linter, [cpplint.py](http://www.rose-hulman.edu/class/csse/csse332/current/resources/cpplint.py).
