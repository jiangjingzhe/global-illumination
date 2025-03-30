#ifndef _DISPLAYFUNC_H
#define	_DISPLAYFUNC_H

#include <math.h>

// Jens's patch for MacOS
#ifdef __APPLE__
#include <GLut/glut.h>
#else
#include <GL/glut.h>
#endif

#include "vec.h"

extern int width;
extern int height;
extern unsigned int *pixels;
extern unsigned int renderingFlags;
extern char captionBuffer[256];

extern int amiSmallptCPU;

extern void InitGlut(int argc, char *argv[], char *windowTittle);
extern double WallClockTime();

extern void ReadScene(char *);
extern void UpdateCamera();

#endif	/* _DISPLAYFUNC_H */

