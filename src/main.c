/*
 * This is a simple Mandelbrot generator program, made to be expanded to a full
 * fractal zoom program at some point. It is more to be an exercise.
 *
 * This file, 'main.c', contains only thing that the user interfaces with
 * directly. Commands, UI, the writing of any output. The libraries report back
 * to 'main.c' with errors, and never print them out on their own. Nothing the
 * end user sees should be handled outside of 'main.c'.
 *
 * Send all complaints and love-letters to bodavelisafrank@gmail.com.
 *
 * Copyright 2017, Maxwell Powlison. Licensed under the GNU GPL v3.0. A copy of
 * this license has been provided in the main directory of this project. If it
 * is missing, a new copy can be downloaded from https://www.gnu.org/.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mandelbrotRender.h"

// For whenever the version number is mentioned by the program.
#define MANDELBROT_VERSION_NUMBER 18
#define MANDELBROT_VERSION_DATE   "2017-05-06"

// Name of the file that the output is saved to.
#define FILENAME "mandelbrot.tga"




// Seperated out for cleanliness.
void helpMenu()
{
    printf(
	"Usage:\n"
	"    mandelbrot [options] width height\n"
	"Available options:\n"
	"        -z : Zoom level.\n"
	"        -x : Real part of the graph center.\n"
	"        -y : Imaginary part of the graph center.\n"
	"        -i : Max iteration count.\n"
	"        -o : Hue offset.\n"
	"        -l : Hue limiter.\n"
	"        -m : Low memory mode (write straight to disk).\n"
	"        -t : Threadcount (overrides lowmem).\n"
	"        -c : Sets a constant brightness value. If set to 0:\n"
	"            -b : Maximum brightness (on a scale of 0 to 1).\n"
	"            -d : Distribution of light (higher -> more spread out).\n"
	"        -v : Version number.\n"
	"        -h : Invokes this help menu.\n"
	);
}




/* The head of the program. Deals with I/O, and passes off gathered arguments to
   the modules for the heavy lifting. */
int main(int argc, char *argv[])
{
    // In case of no arguments.
    if (argc < 2) {
	helpMenu();
	return 1;
    }

    /* 
     * Sets up the default render settings, which may be modified by optargs.
     * 
     * The two main draw settings, offset and zoomlevel, are set to show the
     * whole mandelbrot set by default. The other draw setting, threadCount, is
     * by default 1, as most computers have at least 1 thread nowadays.
     *
     * For the color settings, I've defaulted it to something fairly light on
     * processing power that will produce a fairly decent looking image.
     */
    renderSettings renderInput;
    renderInput.draw.offset.real        = 0;
    renderInput.draw.offset.imag        = 0;
    renderInput.draw.zoomLevel          = 1;
    renderInput.draw.threadCount        = 1;
    renderInput.color.maxIterations     = 360;
    renderInput.color.constantLight     = 0.5;
    renderInput.color.hueOffset         = 0;
    renderInput.color.hueLimiter        = 1;
    renderInput.color.lightMax          = 1;
    renderInput.color.lightDistribution = 4;
    renderInput.calc.juliaFlag          = 0;     // Currently hidden Julia set
    renderInput.calc.juliaConstant.real = -0.8;  // flag and option. Needs 
    renderInput.calc.juliaConstant.imag = 0.156; // cli options for the const.

    // Vars for dealing with optional arguments.
    int arg;               // Holds the current optional arg.
    int lowMemoryFlag = 0; // A flag on whether or not to use low-memory mode.
    int argErrorFlag  = 0; // A flag on whether or not optargs had any failures.

    // Parses optional args (breaks from loop below).
    while (1) {

	// Attempts to get an optarg.
	arg = getopt(argc, argv, "x:y:z:i:o:l:t:b:d:c:mjvh");

	// Quits if there are no more remaining optargs.
	if (arg == -1)
	    break;

	// Parses the argument by matching it with a specific char.
	switch(arg) {
        case 'v':
	    // Show version number.
	    printf(
		"Mandelbrot %d, %s.\n",
		MANDELBROT_VERSION_NUMBER,
		MANDELBROT_VERSION_DATE);
	    return 0;
	    
	case 'x':
	    // 'x' is the real value of the graph center.
	    renderInput.draw.offset.real = atof(optarg);
	    break;
	    
	case 'y':
	    // 'y' is the imag value of the graph center.
	    renderInput.draw.offset.imag = atof(optarg);
	    break;
	    
	case 'z':
	    // 'z' is the zoom multiplier.
	    renderInput.draw.zoomLevel = atof(optarg);
	    break;
	    
	case 'i':
	    // 'i' is the maximum iteration count.
	    renderInput.color.maxIterations = abs(atoi(optarg));
	    break;
	    
	case 'o':
	    // 'o' is the offset of the hue of the image.
	    renderInput.color.hueOffset = atof(optarg);
	    break;
	    
	case 'l':
	    // 'l' is a multiplier of the hue of the image.
	    renderInput.color.hueLimiter = atof(optarg);
	    break;
	    
	case 't':
	    // 't' sets the threadcount.
	    renderInput.draw.threadCount = abs(atoi(optarg));
	    break;

	case 'b':
	    // 'b' sets maximum brightness.
	    renderInput.color.lightMax = atof(optarg);
	    break;

	case 'd':
	    // 'd' sets the light distribution.
	    renderInput.color.lightDistribution = atof(optarg);
	    break;

	case 'c':
	    // 'c' sets whether we are using constant lighting or not.
	    renderInput.color.constantLight = atof(optarg);
	    break;

	case 'h':
	    // 'h' is the help menu. Stops the program as well.
	    helpMenu();
	    return 0;
	    
	case 'm':
	    // 'm' sets low memory mode.
	    lowMemoryFlag = 1;
	    break;

	case 'j':
	    // 'j' sets Julia mode. Renders a Julia set instead of a mandelbrot.
	    renderInput.calc.juliaFlag = 1;
	    break;
	    
	case '?':
	    /* Case of an error in optarg parsing. Checks primarily for options
	       which require arguments, but had none were provided. */
	    if (optopt == 'x')
		fprintf(
		    stderr,
		    "Error: Center real value (-x) not recognized.\n"
		    );
	    
	    else if (optopt == 'y')
		fprintf(
		    stderr,
		    "Error: Center imaginary value (-y) not recognized.\n"
		    );
	    
	    else if (optopt == 'z')
		fprintf(
		    stderr,
		    "Error: Zoom level (-z) not recognized.\n"
		    );
	    
	    else if (optopt == 'i')
		fprintf(
		    stderr,
		    "Error: Iteration count (-i) not recognized.\n"
		    );
	    
	    else if (optopt == 'o')
		fprintf(
		    stderr,
		    "Error: Hue offset value (-o) not recognized.\n"
		    );
	    
	    else if (optopt == 'l')
		fprintf(
		    stderr,
		    "Error: Hue limiter value (-l) not recognized.\n"
		    );

	    else if (optopt == 't')
		fprintf(
		    stderr,
		    "Error: Thread amount (-t) not recognized.\n"
		    );

	    else if (optopt == 'b')
		fprintf(
		    stderr,
		    "Error: Maximum brightness value (-b) not recognized.\n"
		    );

	    else if (optopt == 'd')
		fprintf(
		    stderr,
		    "Error: Light distribution value (-d) not recognized.\n"
		    );

	    else if (optopt == 'c')
		fprintf(
		    stderr,
		    "Error: Constant brightness value (-c) not recognized.\n"
		    );
	    
	    else
		fprintf(
		    stderr,
		    "Error: Option (-%c) not recognized.\n",
		    optopt
		    );

	    // Sets the argument failure flag.
	    argErrorFlag = 1;
	    break;
	    
	default:
	    /* Any normal errors have been caught by the case, so if the code
	       gets to here, there has been an error the code cannot deal with.
	     */
	    abort();
	}
    }

    /* Checks if there are enough non-optional arguments. Sets the arg-error
       flag if not. */
    if (optind > argc - 2) {
	fprintf(
	    stderr,
	    "Error: Resolution is missing one or more arguments.\n"
	    );
	argErrorFlag = 1;
	
    } else {
	// If there are enough arguments for height and width, it retrieves them.
	renderInput.draw.width  = abs(atoi(argv[optind]));
	renderInput.draw.height = abs(atoi(argv[optind+1]));
    }

    

    // Checks for bad arguments.
    if (renderInput.draw.width == 0) {
	/* A negative or 0 width would cause no image to render.
	   A 0 width would also cause FP exceptions (used as a divisor). */
	fprintf(
	    stderr,
	    "Error: Width cannot be 0.\n"
	    );
	
	argErrorFlag = 1;
    }
    
    if (renderInput.draw.height == 0) {
	// A negative or 0 height would cause no image to render.
	fprintf(
	    stderr,
	    "Error: Height cannot be 0.\n"
	    );
	
	argErrorFlag = 1;
    }
    
    if (renderInput.draw.zoomLevel == 0) {
	/* A zoom level of 0 would cause FP exceptions, as the value is used
	   as a divisor.
	*/
	fprintf(
	    stderr,
	    "Error: Cannot have 0 zoom.\n"
	    );
	
	argErrorFlag = 1;
    }
    
    if (renderInput.color.maxIterations < 1) {
	// An iteration count less than 1 can only paint black images.
	fprintf(
	    stderr,
	    "Error: Iteration count cannot be less than 1.\n"
	    );
	
	argErrorFlag = 1;
    }
    
    if (renderInput.draw.threadCount < 1) {
	// A 0 or negative threadcount isn't going to be usable.
	fprintf(
	    stderr,
	    "Error: Threadcount cannot be less than 1.\n"
	    );
	
	argErrorFlag = 1;
    }


    
    /* Exits the program if an input error occured, to prevent abnormal
       behavior.
    */
    if (argErrorFlag == 1) {
	fprintf(
	    stderr,
	    "Use -h for additional help.\n"
	    );
	
	return 1;
    }
    
    

    // Opens up the image to be written to.
    renderInput.imageFile = fopen(FILENAME, "wb");

    // Ensures that the file exists to prevent the program from writing to null.
    if (renderInput.imageFile == NULL) {
	return 3;
    }


    
    /* This section is where the actual rendering occurs, by making calls to
       the library to render the image.
     */
    int status = 0;
    
    /* Renders a Mandelbrot set, either normally, in parallel, or with minimized
       RAM usage. */
    if (renderInput.draw.threadCount > 1)
	status = renderToTarga_parallel(renderInput);
    
    else if (lowMemoryFlag == 0)
	status = renderToTarga(renderInput);
   
    else if (lowMemoryFlag == 1)
	status = renderToTarga_lowMem(renderInput);
    
    else
	fprintf(
	    stderr,
	    "Impossible State: Low-memory flag is invalid (neither 0 or 1).\n"
	    );

    // Closes the targa image.
    fclose(renderInput.imageFile);

    // Checks for memory allocation errors, if memory is allocated.
    if (status == 1 && lowMemoryFlag == 0) {
	fprintf(
	    stderr,
	    "Error: Could not allocate memory for image (approx. %lu bytes).\n",
	    renderInput.draw.height * renderInput.draw.width * 3
	    );
	
	return 2;
    } else if (status == 1 && lowMemoryFlag == 1) {
	fprintf(
	    stderr,
	    "Warning: Abornal exit status from low-memory rendering function.\n"
	    );
	
	return 2;
    }

    // If rendering has been successful, the program exits normally.
    return 0;
}


