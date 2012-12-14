// Patrick Stoica
#include "mylib.h"

u16 *videoBuffer = (u16 *) 0x6000000; // start of the video buffer

// Key state globals
u16 __key_curr = 0, __key_prev = 0;

// A function to set pixel (r, c) to the color passed in
void setPixel(int r, int c, u16 color) {
	videoBuffer[OFFSET(r, c, 240)] = color;
}

// A function to draw a FILLED rectangle starting at (r, c)
void drawRect(int r, int c, int width, int height, u16 color) {
	// we're basically looping through pixel by pixel
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			setPixel(r + i, c + j, color);
		}
	}
}

void waitForVblank() {
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

/* drawImage3
* A function that will draw an arbitrary sized image
* onto the screen (with DMA).
* @param r row to draw the image
* @param c column to draw the image
* @param width width of the image
* @param height height of the image
* @param image Pointer to the first element of the image.
*/
void drawImage3(int r, int c, int width, int height, const u16*
image) {
	for (int x = 0; x < height; x++) {
		DMA[3].src = &image[OFFSET(x, 0, width)];
		DMA[3].dst = &videoBuffer[OFFSET(r + x, c, 240)];
		DMA[3].cnt = (width) | DMA_ON;
	}
}

/* drawRect3
* A DMA Implementation of drawRect
*/
void drawRect3(int r, int c, int width, int height, u16 color) {
	for (int x = 0; x < height; x++) {
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[OFFSET(r + x, c, 240)];
		DMA[3].cnt = (width) | DMA_SOURCE_FIXED | DMA_ON;
	}
}
