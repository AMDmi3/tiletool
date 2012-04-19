/*
 * Copyright (C) 2012 Dmitry Marakasov
 *
 * This file is part of libttip.
 *
 * libttip is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libttip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libttip.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TTIP_INT_H
#define TTIP_INT_H

#include <sys/types.h>

#ifdef WITH_VERBOSE
#include <stdio.h>
#define TTIP_VERROR(fmt,...) fprintf(stderr, "%s: " fmt "\n", __FUNCTION__, ## __VA_ARGS__);
#else
#define TTIP_VERROR(...) do {} while(0)
#endif

#include <ttip.h>

#define TTIP_ALIGN_BITS 2

/* tile structure */
struct ttip_image {
	int width;           /* width in pixels */
	int height;          /* height in pixels */
	int stride;          /* line stride in bytes */
	ttip_format_t format;    /* pixel format */
	unsigned char* data; /* data pointer */
};

/* return number of bytes per pixel for format */
static inline int ttip_getbpp(ttip_format_t format) {
	switch (format) {
	case TTIP_GRAY: return 1;
	case TTIP_GRAY_ALPHA: return 2;
	case TTIP_RGB: return 3;
	case TTIP_RGB_ALPHA: return 4;
	}

	return 0;
}

/* read/write color value to data array */
static inline void ttip_writepixel(unsigned char* ptr, ttip_color_t color, ttip_format_t format) {
	switch (format) {
	case TTIP_GRAY:
		ptr[0] = color;
		break;
	case TTIP_GRAY_ALPHA:
		ptr[0] = color;
		ptr[1] = color >> 8;
		break;
	case TTIP_RGB:
		ptr[0] = color >> 16;
		ptr[1] = color >> 8;
		ptr[2] = color;
		break;
	case TTIP_RGB_ALPHA:
		ptr[0] = color >> 16;
		ptr[1] = color >> 8;
		ptr[2] = color;
		ptr[3] = color >> 24;
		break;
	}
}

static inline ttip_color_t ttip_readpixel(unsigned char* ptr, ttip_format_t format) {
	switch (format) {
	case TTIP_GRAY:
		return ptr[0];
	case TTIP_GRAY_ALPHA:
		return (ptr[0]) | (ptr[1] << 8);
	case TTIP_RGB:
		return (ptr[0] << 16) | (ptr[1] << 8) | (ptr[2]);
	case TTIP_RGB_ALPHA:
		return (ptr[0] << 16) | (ptr[1] << 8) | (ptr[2]) | (ptr[3] << 24);
	}
	return 0;
}

/* alight stride */
static inline size_t ttip_alignstride(size_t stride) {
	size_t lowbitmask = (1 << TTIP_ALIGN_BITS) - 1;
	if (stride & lowbitmask) {
		stride += 1 << TTIP_ALIGN_BITS;
		stride &= ~lowbitmask;
	}

	return stride;
}

static inline unsigned char desaturate(unsigned char r, unsigned char g, unsigned char b) {
	/* desaturate:
	 * v = 0.3 * r + 0.59 * g + 0.11 * b
	 */
	return (r * 19661 + g * 38666 + b * 7209) / 65536;
}

#endif
