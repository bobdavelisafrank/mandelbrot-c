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
#include "mandelbrotRender.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "targa.h"




// Mandelbrot transform, f(z) = z^2 + c, for making an escape-time algorithm.
tComplex mandelbrot(const tComplex c, const tComplex z)
{
    tComplex newZ;
    newZ.real = z.real * z.real - z.imag * z.imag + c.real;
    newZ.imag = 2 * z.real * z.imag + c.imag;

    return newZ;
}




// Finds out how many iterations it takes for a complex point to diverge.
int escapeTime(const int maxIterations, const tComplex c, calcSettings calc)
{
    // If not rendering a Julia set, render a Mandelbrot set.
    if (calc.juliaFlag == 0) {
	tComplex z;
	z.real = 0;
	z.imag = 0;

	// Counts down the number of iterations it takes for a point to escape.
	for (int iterations = maxIterations; iterations > 0; iterations--) {
	    z = mandelbrot(c, z);

	    if ((z.real * z.real + z.imag * z.imag) >= 4)
		return iterations;
	}

	// If the point did not diverge, an empty value is returned.
	return 0;
    }
    
    /*
     * Otherwise, we are rendering a Julia set. This means changing the first
     * argument of the mandelbrot function to a value that is the same for every
     * point, and changing the initial value of Z to that of the input complex
     * value.
     */
    else {
	// Sets the initial value to the complex input value.
	tComplex z = c;

	// Counts down the number of iterations it takes for a point to escape.
	for (int iterations = maxIterations; iterations > 0; iterations--) {
	    z = mandelbrot(calc.juliaConstant, z);

	    if ((z.real * z.real + z.imag * z.imag) >= 4)
		return iterations;
	}

	// If the point did not diverge, an empty value is returned.
	return 0;
    }

    /* 
     * If you ever need to add new Fractal types, modify the calcSettings type
     * to add a new flag (or modify the old flag) to allow setting the new
     * fractal type, and add in any additional parameters needed.
     *
     * Note: I'm aware that this setup means that I'm performing a few million
     * more boolean operations, and that only 1 operation is really necessary;
     * but the image itself already uses around a billion operations by default.
     *
     * It's simpler to check here instead of implementing whole new sets of
     * functions, as this means you don't have to modify anything else about
     * the renderer.
     */
}




// Outputs a color, based on the escape time.
tRGB escapeColor(const int escapeTime,
		 const colorSettings color)
{
    // Unpacks the used color settings.
    int    maxIterations = color.maxIterations;
    double constantLight = color.constantLight;
    double hueLimiter    = color.hueLimiter;
    double hueOffset     = color.hueOffset;
    double lightMax      = color.lightMax;
    double lightDist     = color.lightDistribution;
    
    // RGB color value to return.
    tRGB colorReturn;

    /* Points with zeroed-out escape time are assumed to be in the mandelbrot
       set. */
    if (escapeTime == 0) {
	colorReturn.r = 0;
	colorReturn.g = 0;
	colorReturn.b = 0;
	return colorReturn;
    }

    // Calculates the ratio between the escape time and maximum iteration count.
    const double eTime  = (double) escapeTime;
    const double mIter  = (double) maxIterations;
    const double eRatio = eTime / mIter;

    // The color is being calculated in an HSL color-space.
    double hue;
    double saturation;
    double lightness;

    // Calculates the hue, as a function of the escape-time ratio.
    hue  = 360.0 - 360.0 * eRatio * hueLimiter;
    hue  = fmod(hue, 360.0);
    hue += hueOffset;
    hue  = fmod(hue, 360.0);

    // Calculates the saturation. This is currently a fixed value.
    saturation = 1;

    /* Calculates the lightness, either as a function of escape-time ratio, or
       as a constant value. */
    if (constantLight == 0) {
	lightness   = lightMax * pow(eRatio, lightDist);
	lightness   = lightMax - lightness;
    } else {
	lightness = constantLight;
    }
    
    /* en.wikipedia.org/wiki/HSL_and_HSV#From_HSL -> source of HSL to RGB color
       conversion. */

    // Calculates chroma value of the pixel.
    double chroma;
    chroma  = 1.0 - fabs(2.0 * lightness - 1.0);
    chroma *= saturation;

    // Because smaller numbers look neater. Or something like that.
    hue /= 60.0;

    // Calculates an intermediate value, X.
    double X;
    X  = 1.0 - fabs(fmod(hue, 2.0) - 1.0);
    X *= chroma;

    // Finds out the appropriate RGB values based on Hue.
    double r = 0;
    double g = 0;
    double b = 0;
    if (hue < 1.0) {
	r = chroma;
	g = X;
    } else if (hue < 2.0) {
	r = X;
	g = chroma;
    } else if (hue < 3.0) {
	g = chroma;
	b = X;
    } else if (hue < 4.0) {
	g = X;
	b = chroma;
    } else if (hue < 5.0) {
	r = X;
	b = chroma;
    } else if (hue < 6.0) {
	r = chroma;
	b = X;
    }

    // Calculates an intermediate value, m, to translate the RGB values.
    double m = lightness - 0.5 * chroma;

    // Translates the RGB values.
    r += m;
    b += m;
    g += m;

    // Converts the RGB values to 8 bit integers and saves them to the output.
    r *= 255;
    g *= 255;
    b *= 255;
    colorReturn.r = ((unsigned int) ceil(r)) & 0x00FF;
    colorReturn.g = ((unsigned int) ceil(g)) & 0x00FF;
    colorReturn.b = ((unsigned int) ceil(b)) & 0x00FF;
    
    // Returns the final color value of the pixel.
    return colorReturn;
}




// Calculates an RGB pixel value based off a complex point. 
tRGB escapePixel(const tComplex      c,
		 const colorSettings color,
		 const calcSettings  calc)
{
    // Calculates the escape-time value at the current complex point.
    int eTime = escapeTime(color.maxIterations, c, calc);

    // Calculates the color value based on escape time, and returns it.
    return escapeColor(eTime, color);
}




/* Renders the Mandelbrot set and saves it in a TARGA image format. Returns an
   int to indicate memory allocation failure. */
int renderToTarga(const renderSettings renderInput)
{
    // Unpacks the inputs.
    FILE               *imageFile = renderInput.imageFile;
    const int           width     = renderInput.draw.width;
    const int           height    = renderInput.draw.height;
    const tComplex      offset    = renderInput.draw.offset;
    const double        zoomLevel = renderInput.draw.zoomLevel;
    const colorSettings color     = renderInput.color;
    const calcSettings  calc      = renderInput.calc;

    // Allocates a 2D tRGB array for temporarily storing the image render.
    tRGB **mandelbrot;
    tRGB  *mandelbrotData;
    mandelbrotData = targaAllocateImage(&mandelbrot, width, height);

    // Checks for memory allocation failure, and throws an error status if so.
    if (mandelbrot == NULL || mandelbrotData == NULL)
	return 1;

    /* Pre-calculates the constants needed for mapping the X-Y values of the
       image to the desired locaiton on the complex plane. */
    const double dwidth    = (double) width;
    const double dheight   = (double) height;
    const double step      = 4 / (dwidth * zoomLevel);
    const double realStart = (-2.0 / zoomLevel) + offset.real;
    const double imagStart = (2.0 / zoomLevel) * dheight / dwidth - offset.imag;
    
    // Function for calculating the imaginary part of a point from the X value.
    double scaleX(int x) {
	return realStart + step * x;
    }

    // Function for calculating the real part of a point from the Y value.
    double scaleY(int y) {
	return imagStart - step * y;
    }

    // Renders the mandelbrot to RAM, one pixel at a time.
    for (int y = 0; y < height; y++) {
	for (int x = 0; x < width; x++) {
	    tComplex cursor;
	    cursor.real = scaleX(x);
	    cursor.imag = scaleY(y);
	    
	    // Calculates and saves a 24 bit RGB pixel to the image.
	    mandelbrot[x][y] = escapePixel(cursor, color, calc);
	}
    }

    // Saves the render to a TARGA file for viewing, and deallocates memory.
    targaWriteImage_RGB24(mandelbrot, width, height, imageFile);
    targaDeallocateImage(&mandelbrot, mandelbrotData);

    return 0;
}




// A concurrent version of renderToTarga.
int renderToTarga_parallel(const renderSettings renderInput)
{
    // Unpacks the inputs.
    FILE               *imageFile   = renderInput.imageFile;
    const int           width       = renderInput.draw.width;
    const int           height      = renderInput.draw.height;
    const int           threadCount = renderInput.draw.threadCount;
    const tComplex      offset      = renderInput.draw.offset;
    const double        zoomLevel   = renderInput.draw.zoomLevel;
    const colorSettings color       = renderInput.color;
    const calcSettings  calc        = renderInput.calc;
    
    // Sets the thread count to the input amount.
    omp_set_num_threads(threadCount);

    /* 
     * The multithreading here works by giving each thread a row of the image
     * to render. To prevent cache-swapping, and to keep the malloc calls low,
     * each thread is given its own miniature image that it writes to.
     *
     * When done rendering, the miniature images are "interlaced" together to
     * make the final complete image.
     */
    tRGB **threadImages[threadCount];
    tRGB  *threadImagesData[threadCount];
    const int miniHeight = height / threadCount;

    // Allocates the image for each thread.
    for (int i = 0; i < threadCount; i++) {
	threadImagesData[i] = targaAllocateImage(&threadImages[i],
						 width,
						 miniHeight);

	/* If an image allocation failed, then we deallocate all previously
	   allocated images (if any), and return an error. */
	if (threadImagesData[i] == NULL || threadImages[i] == NULL) {
	    for (int j = 0; j < i; j++)
		targaDeallocateImage(&threadImages[j], threadImagesData[j]);
	    
	    return 1;
	}
    }

    /* Pre-calculates the constants needed for mapping the X-Y values of the
       image to the desired locaiton on the complex plane. */
    const double dwidth    = (double) width;
    const double dheight   = (double) height;
    const double step      = 4 / (dwidth * zoomLevel);
    const double realStart = (-2.0 / zoomLevel) + offset.real;
    const double imagStart = (2.0 / zoomLevel) * dheight / dwidth - offset.imag;
    
    // Function for calculating the imaginary part of a point from the X value.
    double scaleX(int x) {
	return realStart + step * x;
    }

    // Function for calculating the real part of a point from the Y value.
    double scaleY(int y) {
	return imagStart - step * y;
    }

    /* 
     * Starts a parallel block of code. A number of threads execute each 
     * instruction, with the only differences occuring from the use of the
     * thread's unique ID.
     */
    #pragma omp parallel
    {
	// Gets the thread number.
	int threadID = omp_get_thread_num();

	// Copies of external variables are created, to prevent race-conditions.
	tRGB          **threadImage;
	int             threadHeight;
	int             threadWidth;
	int             threadThreadCount;
	colorSettings   threadColor;
	calcSettings    threadCalc;
	
	// Gets needed outside variables in a mutual-exclusion block.
	#pragma omp critical
	{
	    threadImage       = threadImages[threadID];
	    threadHeight      = miniHeight;
	    threadWidth       = width;
	    threadColor       = color;
	    threadCalc        = calc;
	    threadThreadCount = threadCount;
	}
	
	// Renders the mini-image unique to the thread.
	for (int y = 0; y < threadHeight; y++) { 
	    for (int x = 0; x < threadWidth; x++) {
		// Finds the complex point that the X-Y position maps to.
		tComplex threadCursor;
		threadCursor.real = scaleX(x);
		threadCursor.imag = scaleY(y * threadThreadCount + threadID);

		// Calculates and saves a 24 bit RGB pixel to the local image.
		threadImage[x][y] = escapePixel(threadCursor,
						threadColor,
						threadCalc);
	    }
	}
    } // End of parallel code.

    // Writes a header for an uncompressed RGB 24 bit TARGA image to the file.
    targaWriteHeader_RGB24(width, height, imageFile);

    // Writes out each threads' mini-image to the disk, interlaced together.
    for (int y = 0; y < miniHeight; y++)
	for (int i = 0; i < threadCount; i++)
	    for (int x = 0; x < width; x++)
		targaWritePixel_RGB24(threadImages[i][x][y], imageFile);

    // Deallocates the threads' images.
    for (int i = 0; i < threadCount; i++)
	targaDeallocateImage(&threadImages[i], threadImagesData[i]);
    
    return 0;
}




// Variant of renderToTarga, but writes directly to the disk.
int renderToTarga_lowMem(const renderSettings renderInput)
{
    // Unpacks the inputs.
    FILE               *imageFile = renderInput.imageFile;
    const int           width     = renderInput.draw.width;
    const int           height    = renderInput.draw.height;
    const tComplex      offset    = renderInput.draw.offset;
    const double        zoomLevel = renderInput.draw.zoomLevel;
    const colorSettings color     = renderInput.color;
    const calcSettings  calc      = renderInput.calc;

    /* Pre-calculates the constants needed for mapping the X-Y values of the
       image to the desired locaiton on the complex plane. */
    const double dwidth    = (double) width;
    const double dheight   = (double) height;
    const double step      = 4 / (dwidth * zoomLevel);
    const double realStart = (-2.0 / zoomLevel) + offset.real;
    const double imagStart = (2.0 / zoomLevel) * dheight / dwidth - offset.imag;

    // Function for calculating the imaginary part of a point from the X value.
    double scaleX(int x) {
	return realStart + step * x;
    }

    // Function for calculating the real part of a point from the Y value.
    double scaleY(int y) {
	return imagStart - step * y;
    }
    
    // Writes a TARGA header to the file.
    targaWriteHeader_RGB24(width, height, imageFile);
    
    /* Renders the mandelbrot, using a cursor to match the pixel plane to the
       imaginary plane. */
    for (int y = 0; y < height; y++) {
	for (int x = 0; x < width; x++) {
	    // Calculates the complex point.
	    tComplex cursor;
	    cursor.real = scaleX(x);
	    cursor.imag = scaleY(y);
	    
	    // Calculates the 24 bit RGB value at the cursor.
	    tRGB pixel;
	    pixel = escapePixel(cursor, color, calc);
	    
	    // Writes the 24 bit RGB value to the image.
	    targaWritePixel_RGB24(pixel, imageFile);
	}
    }

    // Returns no error.
    return 0;
}



