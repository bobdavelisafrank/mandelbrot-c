
    Send all complaints and love-letters to bodavelisafrank@gmail.com.

    Copyright 2017, Maxwell Powlison. Licensed under the GNU GPL v3.0.

--------------------------------------------------------------------------------
  General program information.
--------------------------------------------------------------------------------

License:
        GNU GPL v3.

Dependencies:
        gcc (or other compatible C compiler)
        openmp
        make

Compilation:
        cd [project directory]/
        make

Installation and Uninstallation:
        As root:
          cd [project directory]/
          make install
          make uninstall

Running:
  Usage:
      mandelbrot [options] width height
  Available options:
          -z : Zoom level.
          -x : Real part of the graph center.
          -y : Imaginary part of the graph center.
          -i : Max iteration count.
          -o : Hue offset.
          -l : Hue limiter.
          -m : Low memory mode (write straight to disk).
          -t : Threadcount (overrides lowmem).
          -c : Sets a constant brightness value. If set to 0:
              -b : Maximum brightness (on a scale of 0 to 1).
              -d : Distribution of light (higher -> more spread out).
          -h : Invokes this help menu.

See "Usage in Detail" for more information on using the program. The general
breakdown is that this program will render a TARGA image, containing a
Mandelbrot set, with width and height set by the input. Various inputs change
how the image is rendred, and how the colors of the image are calculated.

--------------------------------------------------------------------------------
  Program structure information.
--------------------------------------------------------------------------------

project/
        Makefile             -> The primary makefile for the project.
        License              -> Copy of the GNU GPL 3.0, the license used by
                                this project.
        readme               -> This document.
        Usage in Detail      -> A more in-depth look into the use of the
                                program.
        
project/src/
        main.c               -> Program I/O section.
        mandelbrotRender.c/h -> Module for rendering mandelbrot sets.
        targa.c/h            -> Module for creating and handling TARGA images.

'project/' is used as the build directory, and 'project/src/' holds all the
source files.

'main.c' contains only thing that the user interfaces with directly. Commands,
UI, the writing of any output. The libraries report back to 'main.c' with
errors, and never print them out on their own. Nothing the end user sees should
be handled outside of 'main.c'.

'mandelbrotRender.c' contains only things that has to deal with drawing
mandelbrots. All calculations and renderers are put in here. (Modules for image
or video formats are kept separate, however.)

'targa.c' is a module for the TARGA format. 

--------------------------------------------------------------------------------
  A few notes on this program.
--------------------------------------------------------------------------------

You'll not that the code isn't too strongly factored, nor the makefile strongly
automated, nor the majority of the comments useful, nor the syntax entirely
consistent (there's a few breaks over 80 columns and whatnot), nor the interface
strongly defined (apart from the breaks in files). This project was my first 
real foray into C, and a major part of my learning experience for the language.

A few of the other problems with the project as it is now:

1. Multi-threading does not distribute rendering work equally, so it ends up
   averaging about half the possible CPU usage.

2. Run-time switches lack long-name options.

3. The program only outputs to `mandelbrot.tga`, and this output can only be
   encoded in RGB24 TARGA.

4. You only get the two coloring algorithms, with no pallete options existing.

5. According to Valgrind, it leaves 8 bytes on the heap when it finishes.

There are also a few positives about this project:

1. Documentation exists, and was actually written alongside the project! (This
   section being the only exception).

2. Low-memory mode, although incompatible with multi-threading, will always run
   in constant space, using less than 2 megabytes of memory.

3. Even in low-memory mode, the program still runs pretty decently quick. It's 
   not a speed-demon, but given an iteration count of 1024 iterations it will
   perform at around 1.25 to 1.5 megapixels per second (single threaded on an 
   i5-3470).

Enjoy this old bit of programming of mine!
