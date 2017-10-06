# Makefile for the mandelbrot program.
#
# Send all complaints and love-letters to bodavelisafrank@gmail.com.
#
# Copyright 2017, Maxwell Powlison. Licensed under the GNU GPL v3.0.

#-------------------------------------------------------------------------------
# Compatibility checks.
#-------------------------------------------------------------------------------

# Ensures that a compatible shell is used.
SHELL = /bin/sh

# Explicitly sets the suffix list.
.SUFFIXES:
.SUFFIXES: .c .o .h



#-------------------------------------------------------------------------------
# Variable setup.
#-------------------------------------------------------------------------------

INSTALL_PATH ?= /usr/local

# Flags:
#       -g    -> Extra debug info in compiled program.
#       -Wall -> Extra compiler warnings.
#	-O2   -> Optimizes the compiled program. Worth the debug pain here.
#
CC ?= gcc
CFLAGS = -std=c11 -g -Wall -Wextra -Werror -O2

# Libs:
#       -lm      -> Standard math library.
#       -fopenmp -> Parallelization library.
#
LIBS = -lm -fopenmp

# For the compiler to know where the header file is, and for make to know where
# the source files are.
#
INCLUDES = -I $(CURDIR)/src
VPATH = $(CURDIR)/src

# Names of all the object files.
#
OBJ = mandelbrotRender.o targa.o



#-------------------------------------------------------------------------------
# Program compilation.
#-------------------------------------------------------------------------------

# Primary target is an output program titled 'mandelbrot', which gathers
# commandline args and throws them at the functions.
#
default: mandelbrot

# Compiles the I/O section of the program, producing the final executable file.
#
mandelbrot:	main.c $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) $(OBJ) -o $@ $<

# Mandelbrot renderer library.
#
mandelbrotRender.o:	mandelbrotRender.c targa.o mandelbrotRender.h
	$(CC) $(CFLAGS) $(LIBS) -c $<

# TARGA image library.
#
targa.o:	targa.c targa.h
	$(CC) $(CFLAGS) -c $<

#-------------------------------------------------------------------------------
# Program cleaning.
#-------------------------------------------------------------------------------
.PHONY: clean
clean:
	$(RM) mandelbrot *.o *~
	$(RM) $(CURDIR)/src/*~

#-------------------------------------------------------------------------------
# Program install & uninstall.
#-------------------------------------------------------------------------------
.PHONY: install
install:
	mkdir -p $(DESTDIR)$(INSTALL_PATH)/bin
	cp mandelbrot $(DESTDIR)$(INSTALL_PATH)/bin

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(INSTALL_PATH)/bin/mandelbrot

































































































































































































































.PHONY: sandwich
.PHONY: pizza
.PHONY: me
.PHONY: steve
sandwich:
	@echo "Make it yourself."
pizza:
	@echo "No, you can order it."
me:
	@echo "Maybe we can strike a deal."
	@echo "I won't make sandwiches, I won't make pizza,"
	@sleep 2
	@echo "And you can't do anything about it."
	@sleep 3
	@echo "How does that sound?"
steve:
	@echo "Alright, I'll call him."
	@sleep 0.8
	@echo -n "."
	@sleep 0.4
	@echo -n "."
	@sleep 0.4
	@echo -n "."
	@sleep 3
	@echo ""
	@echo -n "Steve? "
	@sleep 0.75
	@echo "Hey man, it's make! How have you --"
	@sleep 2
	@echo "Yeah, I'm just calling in to see how you're doing."
	@sleep 1
	@echo -n "No, "
	@sleep 0.2
	@echo -n "no... "
	@sleep 0.6
	@echo "no other reason."
	@sleep 5
	@echo "He hung up."

.PHONY: love
love:
	@echo "Make it yourself."

.PHONY: war
war:
	@echo "Why not love?"

.PHONY: money
money:
	@echo "make: *** Printing device not found.  Stop."
