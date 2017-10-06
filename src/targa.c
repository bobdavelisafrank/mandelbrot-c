/* 
 * A very simple, non-complete implementation of the TARGA format. This is part
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
#include "targa.h"

#include <stdlib.h>
#include <stdio.h>




/* 
 * Functions for allocating and deallocating memory for a 24 bit RGB image. 
 * Modified from the primary answer to:
 *
 * stackoverflow.com/questions/15062718/allocate-memory-2d-array-in-function-c
 *
 * ...with the primary inclusion of error-checking, as the sources provided in
 * the secondary answer suggest. 
 */
tRGB *targaAllocateImage(tRGB ***image, const int width, const int height)
{
    /* Allocates the first level of the 2D array, which holds pointers to arrays
       that make the rows of the image. Checks for malloc error. */
    *image = malloc(width * sizeof *image);
    if (*image == NULL)
	return NULL;

    // Allocates all the rows on the image in one call. Checks for malloc error.
    tRGB *imageData = malloc(width * height * sizeof (tRGB));
    if (imageData == NULL) {
	free(*image);
	*image = NULL;
	return NULL;
    }

    // Makes the arrays for the rows of the image.
    for(int i = 0; i < width; i++)
	(*image)[i] = imageData + i * height;

    // Returns the interal array. Needs to be saved for clearing memory later.
    return imageData;
}




/* Function for deallocating memory for a 24 bit RGB image. Lightly modified 
   from the same source. */
void targaDeallocateImage(tRGB ***image, tRGB *imageData)
{
    free(imageData);
    imageData = NULL;
    
    free(*image);
    image = NULL;
}




// Writes out a TGA header for an uncompressed 24 bit RGB image.
void targaWriteHeader_RGB24(const int width, const int height, FILE *imageFile)
{
    // TGA formatting lightly modified from paulbourke.net/dataformats/tga.
    
    char header[18];
    header[0]  = 0;                      // id length.
    header[1]  = 0;                      // Colormap type.
    header[2]  = 2;                      // Data-type field. 2 -> Uncompressed RGB.
    header[3]  = 0; header[4]  = 0;      // Colormap origin.
    header[5]  = 0; header[6]  = 0;      // Colormap length.
    header[7]  = 0;                      // Colormap depth.
    header[8]  = 0; header[9]  = 0;      // X origin.
    header[10] = 0; header[11] = 0;      // Y origin.
    header[12] = (width & 0x00FF);       // Image width. First the lower, then the upper byte of the number.
    header[13] = (width & 0xFF00) / 256;
    header[14] = (height & 0x00FF);      // Image height. First the lower, then the upper byte of the number.
    header[15] = (height & 0xFF00) / 256;
    header[16] = 24;                     // Bits per pixel. 24 -> Standard RGB color depth.
    header[17] = 0;                      // Image descriptor.

    // Prints the header to the file.
    for (int i = 0; i < 18; i++)
	fputc(header[i], imageFile);
}




// Prints an RGB pixel to a file.
void targaWritePixel_RGB24(const tRGB pixel, FILE *imageFile) {
    fputc(pixel.b, imageFile);
    fputc(pixel.g, imageFile);
    fputc(pixel.r, imageFile);
}




// Writes out an RGB image to a TGA file.
void targaWriteImage_RGB24(tRGB** image, const int width, const int height, FILE *imageFile)
{
    // Writes the header information to the file.
    targaWriteHeader_RGB24(width, height, imageFile);

    // Prints each pixel to the image. 
    for (int y = 0; y < height; y++)
	for (int x = 0; x < width; x++)
	    targaWritePixel_RGB24(image[x][y], imageFile); 
}
