// C Header File
// Created 6/26/2002; 7:58:45 PM

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>  // Header File For The OpenGL32 Library
#include <OpenGL/glu.h> // Header File For The GLu32 Library
#else
#include <GL/gl.h>      // Header File For The OpenGL32 Library
#include <GL/glu.h>     // Header File For The GLu32 Library
#endif

#ifndef WIN32
#include <unistd.h>
#include <signal.h>
#endif

#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#ifndef COMMON_H
#define COMMON_H

#define		BUT_UP			0x01
#define		BUT_LEFT		0x02
#define		BUT_DOWN		0x04
#define		BUT_RIGHT		0x08
#define		BUT_SHOOT		0x10
#define		BUT_JUMP		0x20
#define		BUT_SWITCH		0x40
#define		BUT_PAUSE		0x80

#define		COLLIDE_ABOVE	0x01
#define		COLLIDE_BELOW	0x02
#define		COLLIDE_LEFT	0x04
#define		COLLIDE_RIGHT	0x08

// 16.15 fixed-point type for locations:
typedef	double	fixed;

// Define a fixed point 16.15 long integer.
// a is the 16-bit signed integer part
// b is the 15-bit unsigned integer fractional part from 0 to 32767
// if you wish to define a negative fixed-point number, use the negative
// sign in front of the FIX macro, not as the integer part (a).
//    -FIX(1, 0)	=	-1.0
//    FIX(1, 16384)	=	1.5
//#define FIX(a,b) (((long)(a) << 15) | (long)(b))
#define	FIX(a,b) (a + (b / 32768.0))

// Determine the sign bit (0 is positive, 1 is negative)
//#define SGN(a) ( ((unsigned long)(a)) & 0x80000000 )

// Absolute value masks off the sign bit:
//#define ABS(a) ( (a) < 0 ? -(a) : (a) )
#define ABS(a)	fabs(a)

//#define		MULT(a, b)		( ((a >> 15) * b) + (((a & 32767) * b) >> 15) )
#define	MULT(a, b)	(a * b)

#endif
