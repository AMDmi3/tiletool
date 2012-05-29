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

#ifndef TTIP_H
#define TTIP_H

#ifdef __cplusplus
extern "C" {
#endif

/* pixel formats */
typedef enum {
	TTIP_GRAY = 1,
	TTIP_GRAY_ALPHA = 2,
	TTIP_RGB = 3,
	TTIP_RGB_ALPHA = 4,

	/* aliases */
	TTIP_GRAYA = TTIP_GRAY_ALPHA,
	TTIP_RGBA = TTIP_RGB_ALPHA,
} ttip_format_t;

/* error codes */
typedef enum {
	TTIP_OK = 0,
	TTIP_LIBPNG_INIT_FAILED = -1,
	TTIP_LIBPNG_ERROR = -2,
	TTIP_NOT_IMPLEMENTED = -3,
	TTIP_IMAGE_FORMAT_NOT_SUPPORTED = -4,
	TTIP_NOT_COMPILED_IN = -5,
	TTIP_BAD_DIMENSIONS = -6,
	TTIP_BAD_PIXEL_FORMAT = -7,
	TTIP_IMAGE_FORMAT_MISMATCH = -8,
	TTIP_EVEN_DIMENSIONS_REQUIRED = -9,

	TTIP_LAST_ERROR = -9,
} ttip_result_t;

/* opaque type for single tile */
typedef struct ttip_image* ttip_image_t;

/* color value for manual color operations
 * format is as follows:
 * ttip_color g = 0xGG
 * ttip_color ga = 0xAAGG;
 * ttip_color rgb = 0xRRGGBB;
 * ttip_color rgba = 0xAARRGGBB;
 */
typedef unsigned int ttip_color_t;

/* tile creation and destruction */
ttip_result_t ttip_create(ttip_image_t* output, unsigned int width, unsigned int height, ttip_format_t format);
void ttip_destroy(ttip_image_t* tile);

/* error handling */
const char* ttip_strerror(ttip_result_t error);

/* property inspection */
int ttip_getwidth(ttip_image_t tile);
int ttip_getheight(ttip_image_t tile);
ttip_format_t ttip_getformat(ttip_image_t tile);

int ttip_issameformat(ttip_image_t a, ttip_image_t b);
int ttip_issamedimension(ttip_image_t a, ttip_image_t b);

/* lowlevel pixel operations */
void ttip_setpixel(ttip_image_t tile, unsigned int x, unsigned int y, ttip_color_t color);
ttip_color_t ttip_getpixel(ttip_image_t tile, unsigned int x, unsigned int y);

/* basic operations */
ttip_result_t ttip_clear(ttip_image_t target);
ttip_result_t ttip_copy(ttip_image_t target, ttip_image_t source);
ttip_result_t ttip_clone(ttip_image_t* output, ttip_image_t source);

/* png input/output */
ttip_result_t ttip_loadpng_inplace(ttip_image_t target, const char* filename);
ttip_result_t ttip_loadpng(ttip_image_t* output, const char* filename);
ttip_result_t ttip_savepng(ttip_image_t source, const char* filename, int level /* = 6 */);

/* transformations */
ttip_result_t ttip_desaturate_inplace(ttip_image_t target, ttip_image_t source);
ttip_result_t ttip_desaturate(ttip_image_t* output, ttip_image_t source);

ttip_result_t ttip_downsample2x2(ttip_image_t* output, ttip_image_t topleft, ttip_image_t topright, ttip_image_t bottomleft, ttip_image_t bottomright);
ttip_result_t ttip_maskblend(ttip_image_t* output, ttip_image_t background, ttip_image_t overlay);
ttip_result_t ttip_threshold(ttip_image_t* output, ttip_image_t source, int value);

#ifdef __cplusplus
}
#endif

#endif
