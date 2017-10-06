/* A very simple, non-complete implementation of the TARGA format. This is part
 * of an exercise program, which draws mandelbrot sets.
 *
 * This module only contains things directly partaining to the TARGA format.
 *
 * Send all complaints and love-letters to bodavelisafrank@gmail.com.
 *
 * Copyright 2017, Maxwell Powlison. Licensed under the GNU GPL v3.0. A copy of
 * this license has been provided in the main directory of this project. If it
 * is missing, a new copy can be downloaded from https://www.gnu.org/.
 */
#ifndef TARGA_MODULE
#define TARGA_MODULE

#include <stdio.h>


// Packed 24-bit RGB type.
typedef struct {
    unsigned int r:8;
    unsigned int g:8;
    unsigned int b:8;
} tRGB;



// Tools for creating an image in RAM.
tRGB *targaAllocateImage(tRGB ***image, const int width, const int height);
void  targaDeallocateImage(tRGB ***image, tRGB *imageData);



// Tools for writing an image to disk.
void targaWriteHeader_RGB24(const int width, const int height, FILE *imageFile);
void targaWritePixel_RGB24(const tRGB pixel, FILE *imageFile);



// Tools for writing an allocated image to disk.
void targaWriteImage_RGB24(tRGB **image, const int width, const int height,
			   FILE *imageFile);



#endif /* TARGA_MODULE */
