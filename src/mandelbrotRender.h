/* 
 * This is a module for rendering mandelbrot sets, part of an exercise program
 * that draws mandelbrot sets.
 *
 * This file, 'mandelbrotRender.c' contains only things that has to deal with 
 * drawing mandelbrots. All calculations and renderers are put in here. Modules 
 * for image or video formats are kept separate, however.
 *
 * Send all complaints and love-letters to bodavelisafrank@gmail.com.
 *
 * Copyright 2017, Maxwell Powlison. Licensed under the GNU GPL v3.0. A copy of
 * this license has been provided in the main directory of this project. If it
 * is missing, a new copy can be downloaded from https://www.gnu.org/.
 */
#ifndef MANDELBROT_RENDER_MODULE
#define MANDELBROT_RENDER_MODULE

#include <stdio.h>



// A type for complex numbers.
typedef struct {
    double real; // Real part of the complex number.
    double imag; // Imaginary part of the complex number.
} tComplex;



// Customizable settings for the low-level calculations of the image.
typedef struct {
    // Tells the renderer whether or not it should render a Julia set.
    int      juliaFlag;
    /* If the renderer is rendering a Julia set, this holds the fixed value
       describing the set. */
    tComplex juliaConstant;
} calcSettings;



// Customizable settings for how the renderer creates and maps the image.
typedef struct {
    unsigned long int width;
    unsigned long int height;
    unsigned int      threadCount;
    tComplex          offset; // Place in complex plane the image is centered onto.
    double            zoomLevel;
} drawSettings;



// Customizable settings for how the renderer draws the image.
typedef struct {
    int    maxIterations;
    // Sets a multiplier for limiting the hue spectrum used.
    double hueLimiter;
    // Sets an offset for rotating the hue spectrum.
    double hueOffset;
    // Gives a lightness value to use.
    double constantLight;
    /* If constantLight is set to 0, then lightness becomes a funciton of
     * escape time, and these parameters change by what effect. 
     *
     * lightMax -> determines maximum lightness value that any pixel can
     *             reach.
     * lightDistribution -> determines how spread out the light is.
     */
    double lightMax;
    double lightDistribution;
} colorSettings;



/*
 * A single struct for packing in the numerous arguments for the renderer.
 *
 * The struct itself is passed to the renderer, where it unpacks the drawing
 * settings to construct the image, and passes the color settings to the
 * calculation functions. 
 *
 * This allows for semi-modularity of inputs. I can change how color settings
 * are customized without having to change the rendering functions themselves,
 * for example.
 */
typedef struct {
    drawSettings  draw;
    colorSettings color;
    calcSettings  calc;
    FILE         *imageFile;
} renderSettings;



/* 
 * Rendering functions. They draw mandelbrots to a given file.
 *
 * For the simplicity of keeping the image API out of the I/O code, these
 * functions are rather heavy-handed in what they do.
 */
int renderToTarga(const renderSettings renderInput);
int renderToTarga_parallel(const renderSettings renderInput);
int renderToTarga_lowMem(const renderSettings renderInput);

#endif // MANDELBROT_RENDER_MODULE
