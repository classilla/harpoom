//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	Endianess handling, swapping 16bit and 32bit.
//


#ifndef __I_SWAP__
#define __I_SWAP__

#ifdef FEATURE_SOUND


#ifdef __DJGPP__


#define SHORT(x)  ((signed short) (x))
#define LONG(x)   ((signed int) (x))

#define SYS_LITTLE_ENDIAN


#else  // __DJGPP__


#include <SDL_endian.h>

// Endianess handling.
// WAD files are stored little endian.

// Just use SDL's endianness swapping functions.

// These are deliberately cast to signed values; this is the behaviour
// of the macros in the original source and some code relies on it.

#define SHORT(x)  ((signed short) SDL_SwapLE16(x))
#define LONG(x)   ((signed int) SDL_SwapLE32(x))

// Defines for checking the endianness of the system.

#if SDL_BYTEORDER == SYS_LIL_ENDIAN
#define SYS_LITTLE_ENDIAN
#elif SDL_BYTEORDER == SYS_BIG_ENDIAN
#define SYS_BIG_ENDIAN
#endif

// cosmito from lsdldoom
#define doom_swap_s(x) \
        ((short int)((((unsigned short int)(x) & 0x00ff) << 8) | \
                              (((unsigned short int)(x) & 0xff00) >> 8))) 


#if ( SDL_BYTEORDER == SDL_BIG_ENDIAN )
#define doom_wtohs(x) doom_swap_s(x)
#else
#define doom_wtohs(x) (short int)(x)
#endif


#endif  // __DJGPP__


#else  // FEATURE_SOUND

static inline signed int LONG(signed int x) {
	signed int result;
	__asm__(
		"rotlwi %0, %1, 8\n"
		"rlwimi %0, %1, 24, 0, 7\n"
		"rlwimi %0, %1, 24, 16, 23\n"
		/* note: 64 bit would need an extsw here */

		/* out */ : "=&r" (result)
		/* in  */ : "r"  (x));
	return result;
}

static inline signed short SHORT(signed short x) {
	signed short result;
	__asm__(
		"rlwinm %0, %1, 24, 24, 31\n"
		"rlwinm 0, %1, 8, 16, 23\n"
		"or %0, %0, 0\n"
		"extsh %0, %0\n"

		/* out */ : "=&r" (result)
		/* in  */ : "r" (x)
		/* temp */: "0");
	return result;
}

#define SYS_BIG_ENDIAN

#endif /* FEATURE_SOUND */

#endif

